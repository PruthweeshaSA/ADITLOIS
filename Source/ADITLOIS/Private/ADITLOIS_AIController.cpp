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
	NavWaypoint.Reset();
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

FVector AADITLOIS_AIController::GetNavWaypoint(FVector TargetLocation)
{
	APawn *ControlledPawn = GetPawn();

	if (NavSys && ControlledPawn)
	{
		// Calculate path without moving
		UNavigationPath *CalculatedPath = NavSys->FindPathToLocationSynchronously(
			this,								// World Context
			ControlledPawn->GetActorLocation(), // Start
			TargetLocation						// End
		);

		if (CalculatedPath && CalculatedPath->IsValid())
		{
			// TArray<FVector> containing Start -> Corners -> End
			TArray<FVector> Points = CalculatedPath->PathPoints;

			FVector NewNavWaypoint = (Points.Num() > 1) ? Points[1] : TargetLocation;

			DrawDebugSphere(this->GetWorld(), NewNavWaypoint, 50.0f, 1.0, FColor::Magenta, false, 10.0f);
			return NewNavWaypoint;
		}
	}

	return TargetLocation;
}

FVector AADITLOIS_AIController::GetIdealWaypoint()
{
	APawn *ControlledPawn = GetPawn();
	if (!ControlledPawn)
		return FVector::ZeroVector;

	if (gameState && !(gameState->GetIsMovementConstrained()))
	{
		return (NavWaypoint.IsSet()) ? NavWaypoint.GetValue() : GetPawn()->GetActorLocation();
	}

	FVector PrimaryAxis = FVector(1.0, 0.0, 0.0);
	FVector SecondaryAxis = FVector(0.0, 1.0, 0);

	if (NavWaypoint.IsSet())
	{
		FVector differenceVector = NavWaypoint.GetValue() - ControlledPawn->GetActorLocation();
		FVector PrimaryComponentProjection = differenceVector.ProjectOnTo(PrimaryAxis);
		FVector SecondaryComponentProjection = differenceVector.ProjectOnTo(SecondaryAxis);
		FVector NewOrthoWaypoint = ControlledPawn->GetActorLocation();

		float PrimaryFirstCost = -1.0f;
		float SecondaryFirstCost = -1.0f;

		UNavigationPath *NavPathToPrimaryWaypoint = nullptr;
		UNavigationPath *NavPathFromPrimaryWaypoint = nullptr;
		UNavigationPath *NavPathToSecondaryWaypoint = nullptr;
		UNavigationPath *NavPathFromSecondaryWaypoint = nullptr;


		if (NavSys && ControlledPawn)
		{
			FVector PrimaryFirstWaypoint = ControlledPawn->GetActorLocation() + PrimaryComponentProjection;
			FVector SecondaryFirstWaypoint = ControlledPawn->GetActorLocation() + SecondaryComponentProjection;


			// Raycast returns TRUE if there is an OBSTRUCTION (a hit)
			// Raycast returns FALSE if the path is clear
			FVector PrimaryMargin = (PrimaryComponentProjection.GetSafeNormal() * 3.0 * ACCEPTANCE_RADIUS);
			FVector SecondaryMargin = (SecondaryComponentProjection.GetSafeNormal() * 3.0 * ACCEPTANCE_RADIUS);
			FVector HitLocation;
			bool bPrimaryHit = NavSys->NavigationRaycast(this, ControlledPawn->GetActorLocation(), PrimaryFirstWaypoint, HitLocation);
			PrimaryFirstWaypoint = (bPrimaryHit) ? HitLocation - PrimaryMargin : PrimaryFirstWaypoint + PrimaryMargin;
			bool bSecondaryHit = NavSys->NavigationRaycast(this, ControlledPawn->GetActorLocation(), SecondaryFirstWaypoint, HitLocation);
			SecondaryFirstWaypoint = (bSecondaryHit) ? HitLocation - SecondaryMargin : SecondaryFirstWaypoint + SecondaryMargin;

			NewOrthoWaypoint = (PrimaryComponentProjection.Size() > SecondaryComponentProjection.Size()) ? PrimaryFirstWaypoint : SecondaryFirstWaypoint;

			NavPathToPrimaryWaypoint = NavSys->FindPathToLocationSynchronously(this->GetWorld(), ControlledPawn->GetActorLocation(), PrimaryFirstWaypoint);
			NavPathFromPrimaryWaypoint = NavSys->FindPathToLocationSynchronously(this->GetWorld(), PrimaryFirstWaypoint, NavWaypoint.GetValue());

			if (NavPathToPrimaryWaypoint && NavPathToPrimaryWaypoint->IsValid() && NavPathFromPrimaryWaypoint && NavPathFromPrimaryWaypoint->IsValid())
			{
				PrimaryFirstCost = NavPathToPrimaryWaypoint->GetPathLength() + NavPathFromPrimaryWaypoint->GetPathLength();
			}

			NavPathToSecondaryWaypoint = NavSys->FindPathToLocationSynchronously(this->GetWorld(), ControlledPawn->GetActorLocation(), SecondaryFirstWaypoint);
			NavPathFromSecondaryWaypoint = NavSys->FindPathToLocationSynchronously(this->GetWorld(), SecondaryFirstWaypoint, NavWaypoint.GetValue());

			if (NavPathToSecondaryWaypoint && NavPathToSecondaryWaypoint->IsValid() && NavPathFromSecondaryWaypoint && NavPathFromSecondaryWaypoint->IsValid())
			{
				SecondaryFirstCost = NavPathToSecondaryWaypoint->GetPathLength() + NavPathFromSecondaryWaypoint->GetPathLength();
			}

			if (ControlledPawn->GetVelocity().Size() > 0.01f)
			{
				NewOrthoWaypoint = (FMath::Abs(FVector::DotProduct(ControlledPawn->GetVelocity(), PrimaryAxis)) > FMath::Abs(FVector::DotProduct(ControlledPawn->GetVelocity(), SecondaryAxis))) ? PrimaryFirstWaypoint : SecondaryFirstWaypoint;
				return NewOrthoWaypoint;
			}
			if (NavPathToPrimaryWaypoint->GetPathLength() > 0.0f && NavPathToSecondaryWaypoint->GetPathLength() > 0.0f)
			{
				NewOrthoWaypoint = (NavPathFromPrimaryWaypoint->GetPathLength() < NavPathFromSecondaryWaypoint->GetPathLength()) ? PrimaryFirstWaypoint : SecondaryFirstWaypoint;
			}
		}

		return NewOrthoWaypoint;
	}
	else
	{
		return GetPawn()->GetActorLocation();
	}
}

void AADITLOIS_AIController::ScanForInteractables()
{
	LastScannedTimestamp = GetWorld()->GetTimeSeconds();
	APawn *ControlledPawn = GetPawn();
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
		
		NavWaypoint = GetNavWaypoint(BestTarget->GetActorLocation());
		OrthoNavWaypoint = (NavWaypoint.IsSet()) ? GetIdealWaypoint() : FVector::ZeroVector;
		DrawDebugSphere(this->GetWorld(), OrthoNavWaypoint.GetValue(), 50.0f, 1.0, FColor::Blue, false, 10.0f);

		MoveToLocation(OrthoNavWaypoint.GetValue(), ACCEPTANCE_RADIUS);
		
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