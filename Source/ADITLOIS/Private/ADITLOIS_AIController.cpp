#include "ADITLOIS_AIController.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "IADITLOIS_Interactable_Interface.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h" // Required for SphereOverlap
#include "ADITLOIS_GameState.h"

AADITLOIS_AIController::AADITLOIS_AIController()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentTargetActor = nullptr;
	TargetLocation.Reset();
	OrthoNavWaypoint.Reset();
}

void AADITLOIS_AIController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("AI Controller BeginPlay called."));
	gameState = Cast<AADITLOIS_GameState>(GetWorld()->GetGameState());
}

void AADITLOIS_AIController::OnPossess(APawn *aPawn)
{
	Super::OnPossess(aPawn);
	UE_LOG(LogTemp, Log, TEXT("AI Controller OnPossess called."));

	ControlledPawn = GetPawn();

	if (ACharacter *PossessedCharacter = Cast<ACharacter>(aPawn))
	{
		if (UCharacterMovementComponent *MoveComp = PossessedCharacter->GetCharacterMovement())
		{
			MoveComp->bOrientRotationToMovement = true;
		}
	}

	// --- FIX: Set Timer ONLY ONCE here. ---
	// Since bLoop is true, we never need to set this again.
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ScanTimerHandle,
			this,
			&AADITLOIS_AIController::ScanForInteractables,
			ScanInterval,
			true // Loop forever
		);

		NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	}
}

void AADITLOIS_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AADITLOIS_AIController::GetIdealWaypoint()
{
	if (!ControlledPawn)
		return FVector::ZeroVector;

	if (gameState && !(gameState->GetIsMovementConstrained()))
	{
		return (TargetLocation.IsSet()) ? TargetLocation.GetValue() : GetPawn()->GetActorLocation();
	}

	FVector PrimaryAxis = FVector(1.0, 0.0, 0.0);
	FVector SecondaryAxis = FVector(0.0, 1.0, 0);

	if (TargetLocation.IsSet())
	{
		FVector ToNav = TargetLocation.GetValue() - GetPawn()->GetActorLocation();
		FVector PrimaryComponent = ToNav.ProjectOnTo(PrimaryAxis);
		FVector SecondaryComponent = ToNav.ProjectOnTo(SecondaryAxis);

		FVector PrimaryCandidate = PrimaryComponent.GetSafeNormal() * (200.0f * ACCEPTANCE_RADIUS) + GetPawn()->GetActorLocation();
		FVector SecondaryCandidate = SecondaryComponent.GetSafeNormal() * (200.0f * ACCEPTANCE_RADIUS) + GetPawn()->GetActorLocation();
		FVector AntiPrimaryCandidate = -PrimaryComponent.GetSafeNormal() * (200.0f * ACCEPTANCE_RADIUS) + GetPawn()->GetActorLocation();
		FVector AntiSecondaryCandidate = -SecondaryComponent.GetSafeNormal() * (200.0f * ACCEPTANCE_RADIUS) + GetPawn()->GetActorLocation();

		float PrimaryCost = 0.0f;
		float SecondaryCost = 0.0f;
		float AntiPrimaryCost = 0.0f;
		float AntiSecondaryCost = 0.0f;

		FVector HitLocation;
		for (int i = 0; i < 4; i++)
		{
			FVector Candidate;
			float CandidateCost;
			switch (i)
			{
			case 0:
				Candidate = PrimaryCandidate;
				break;
			case 1:
				Candidate = SecondaryCandidate;
				break;
			case 2:
				Candidate = AntiPrimaryCandidate;
				break;
			case 3:
				Candidate = AntiSecondaryCandidate;
				break;
			}

			bool bHitCandidate = NavSys->NavigationRaycast(GetWorld(), GetPawn()->GetActorLocation(), Candidate, HitLocation, nullptr);
			if (!bHitCandidate)
			{
				NavSys->GetPathLength(Candidate, TargetLocation.GetValue(), CandidateCost);
				switch (i)
				{
				case 0:
					PrimaryCost = CandidateCost;
					break;
				case 1:
					SecondaryCost = CandidateCost;
					break;
				case 2:
					AntiPrimaryCost = CandidateCost;
					break;
				case 3:
					AntiSecondaryCost = CandidateCost;
					break;
				}
			}
			else
			{
				NavSys->GetPathLength(HitLocation, TargetLocation.GetValue(), CandidateCost);
				bool bHitTooClose = FVector::DistSquared(GetPawn()->GetActorLocation(), Candidate) < FMath::Square(ACCEPTANCE_RADIUS);
				switch (i)
				{
				case 0:
					PrimaryCost = bHitTooClose ? MAX_FLT : CandidateCost;
					break;
				case 1:
					SecondaryCost = bHitTooClose ? MAX_FLT : CandidateCost;
					break;
				case 2:
					AntiPrimaryCost = bHitTooClose ? MAX_FLT : CandidateCost;
					break;
				case 3:
					AntiSecondaryCost = bHitTooClose ? MAX_FLT : CandidateCost;
					break;
				}
			}
		}

		FVector PawnVelocity = ControlledPawn->GetVelocity();
		int BestIndex = 0;
		if (!PawnVelocity.IsNearlyZero())
		{
			FVector VelocityDir = PawnVelocity.GetSafeNormal();
			float DotPrimary = FVector::DotProduct(VelocityDir, PrimaryComponent.GetSafeNormal());
			float DotSecondary = FVector::DotProduct(VelocityDir, SecondaryComponent.GetSafeNormal());

			BestIndex = (DotPrimary > 0.5) ? 0 : (DotSecondary > 0.5) ? 1
											 : (DotPrimary < -0.5)	  ? 2
																	  : 3;
		}

		std::vector<float> Costs = {PrimaryCost, SecondaryCost, AntiPrimaryCost, AntiSecondaryCost};
		std::vector<FVector> Candidates = {PrimaryCandidate, SecondaryCandidate, AntiPrimaryCandidate, AntiSecondaryCandidate};

		for (int i = 0; i < Costs.size(); i++)
		{
			if (Costs[i] < Costs[BestIndex] - 100.0f)
			{
				BestIndex = i;
			}
		}
		FVector BestCandidate = Candidates[BestIndex];

		return BestCandidate;
	}
	else
	{
		return GetPawn()->GetActorLocation();
	}
}

void AADITLOIS_AIController::ScanForInteractables()
{
	LastScannedTimestamp = GetWorld()->GetTimeSeconds();
	if (!ControlledPawn)
		return;

	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	AActor *BestTarget = nullptr;
	float ClosestDistSq = MAX_FLT;

	// --- OPTIMIZATION: Use SphereOverlap instead of Iterator ---
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor *> OverlappedActors;
	TArray<AActor *> ActorsToIgnore;
	ActorsToIgnore.Add(ControlledPawn);

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		CurrentLocation,
		ScanRadius,
		ObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OverlappedActors);

	for (AActor *Actor : OverlappedActors)
	{
		if (Actor && Actor->Implements<UADITLOIS_Interactable_Interface>())
		{
			float DistSq = FVector::DistSquared(CurrentLocation, Actor->GetActorLocation());
			if (DistSq < ClosestDistSq)
			{
				ClosestDistSq = DistSq;
				BestTarget = Actor;
			}
		}
	}

	// --- LOGIC: Move or Roam ---

	float AcceptanceRadiusSq = 400.0f * 400.0f;

	if (BestTarget)
	{
		// 1. If close enough, interact
		if (ClosestDistSq <= AcceptanceRadiusSq)
		{
			StopMovement();
			IADITLOIS_Interactable_Interface::Execute_Interact(BestTarget, ControlledPawn);
			CurrentTargetActor = nullptr; // Reset tracking
			return;
		}

		// 2. If valid target, move to it
		if (CurrentTargetActor != BestTarget)
		{
			CurrentTargetActor = BestTarget;
			UE_LOG(LogTemp, Log, TEXT("AI: Found new target %s, moving."), *BestTarget->GetName());
		}

		TargetLocation = BestTarget->GetActorLocation();
		OrthoNavWaypoint = (TargetLocation.IsSet()) ? GetIdealWaypoint() : FVector::ZeroVector;
		DrawDebugSphere(this->GetWorld(), OrthoNavWaypoint.GetValue(), 50.0f, 1.0, FColor::Blue, false, 10.0f);

		MoveToLocation(OrthoNavWaypoint.GetValue(), 1.0f);
	}
	else
	{
		CurrentTargetActor = nullptr;

		// 3. If no target and NOT moving, find a random point
		if (GetMoveStatus() == EPathFollowingStatus::Idle)
		{
			FNavLocation RandomLocation;
			UNavigationSystemV1 *NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(CurrentLocation, ScanRadius, RandomLocation))
			{
				MoveToLocation(RandomLocation.Location);
				UE_LOG(LogTemp, Log, TEXT("AI: Roaming to random location."));
			}
		}
	}
}

void AADITLOIS_AIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult &Result)
{
	if ((LastScannedTimestamp + 0.5) < GetWorld()->GetTimeSeconds())
	{
		ScanForInteractables();
		UE_LOG(LogTemp, Log, TEXT("AIController: Movement completed, restarting scan."));
	}
	else
	{
		// ScanForInteractables();
		UE_LOG(LogTemp, Log, TEXT("AIController: Movement completed, scan recently done, skipping."));
	}
}