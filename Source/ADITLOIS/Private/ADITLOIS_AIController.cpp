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
	}
}

void AADITLOIS_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn && ControlledPawn->GetVelocity().Size() < 0.01f && OrthoNavWaypoint.IsSet() && OrthoNavWaypoint.GetValue().Equals(ControlledPawn->GetActorLocation(), ACCEPTANCE_RADIUS))
	{
		DrawDebugSphere(this->GetWorld(), ControlledPawn->GetActorLocation(), 50.0f, 1.0, FColor::Yellow, false, 10.0f);
		MoveToLocation(ControlledPawn->GetActorLocation() + FVector(400.0f, 0.0f, 0.0f), ACCEPTANCE_RADIUS);
	}
}


FVector AADITLOIS_AIController::GetNavWaypoint(FVector TargetLocation)
{
	UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	APawn* ControlledPawn = GetPawn();

	if (NavSys && ControlledPawn)
	{
		// Calculate path without moving
		UNavigationPath *CalculatedPath = NavSys->FindPathToLocationSynchronously(
			this,						   // World Context
			ControlledPawn->GetActorLocation(), // Start
			TargetLocation				   // End
		);

		if (CalculatedPath && CalculatedPath->IsValid())
		{
			// TArray<FVector> containing Start -> Corners -> End
			TArray<FVector> Points = CalculatedPath->PathPoints;

			FVector NewNavWaypoint = (Points.Num() > 1) ? Points[1] : TargetLocation;

			DrawDebugSphere(this->GetWorld(),NewNavWaypoint, 50.0f, 1.0, FColor::Blue, false, 10.0f);
			return NewNavWaypoint;
		}
	}

	return TargetLocation;
}

FVector AADITLOIS_AIController::GetNavigableOrthoWaypoint(FVector TargetLocation)
{
	UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	APawn* ControlledPawn = GetPawn();

	if (NavSys && ControlledPawn)
	{
		// Raycast returns TRUE if there is an OBSTRUCTION (a hit)
		// Raycast returns FALSE if the path is clear
		FVector CurrentLocation = ControlledPawn->GetActorLocation();
		FVector NewNavigableLocation;
		bool bHitWall = NavSys->NavigationRaycast(this, CurrentLocation, TargetLocation, NewNavigableLocation);

		if (bHitWall)
		{
			DrawDebugSphere(this->GetWorld(),NewNavigableLocation, 50.0f, 1.0, FColor::Red, false, 10.0f);
			return NewNavigableLocation;
		}
		else
		{
			return TargetLocation + (TargetLocation - CurrentLocation).GetSafeNormal()*(5.0f*ACCEPTANCE_RADIUS);
		}
	}
	return TargetLocation;
}

FVector AADITLOIS_AIController::GetOrthoWaypoint(FVector TargetLocation)
{
	if (gameState && !(gameState->GetIsMovementConstrained()))
	{
		return TargetLocation;
	}
	
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return TargetLocation;

	FVector PrimaryAxis = FVector(1.0, 0.0, 0.0);
	FVector SecondaryAxis = FVector(0.0, 1.0, 0);
	if (NavWaypoint.IsSet())
	{
		FVector differenceVector = NavWaypoint.GetValue() - ControlledPawn->GetActorLocation();
		FVector PrimaryComponentProjection = differenceVector.ProjectOnTo(PrimaryAxis);
		FVector SecondaryComponentProjection = differenceVector.ProjectOnTo(SecondaryAxis);
		FVector NewOrthoWaypoint;

		if (SecondaryComponentProjection.Size2D() == 0)
		{	
			NewOrthoWaypoint = GetNavigableOrthoWaypoint(ControlledPawn->GetActorLocation() + PrimaryComponentProjection);
		}
		else if (PrimaryComponentProjection.Size2D() == 0)
		{
			NewOrthoWaypoint = GetNavigableOrthoWaypoint(ControlledPawn->GetActorLocation() + SecondaryComponentProjection);
		}
		else
		{
			float RandomFloat = FMath::FRandRange(0.0f, 1.0f);

			FVector LongerComponent = (PrimaryComponentProjection.Size2D() > SecondaryComponentProjection.Size2D())? PrimaryComponentProjection : SecondaryComponentProjection;
			FVector ShorterComponent = (PrimaryComponentProjection.Size2D() > SecondaryComponentProjection.Size2D())? SecondaryComponentProjection : PrimaryComponentProjection;

			FVector LongerLegFirstWaypoint = GetNavigableOrthoWaypoint(ControlledPawn->GetActorLocation() + LongerComponent);
			FVector ShorterLegFirstWaypoint = GetNavigableOrthoWaypoint(ControlledPawn->GetActorLocation() + ShorterComponent);

			FVector PreferredOrthoWaypoint = LongerLegFirstWaypoint;
			FVector AlternativeOrthoWaypoint = ShorterLegFirstWaypoint;

			UNavigationSystemV1 *NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			FVector PotentialHitPoint;

			if (FVector::DotProduct((ShorterLegFirstWaypoint - ControlledPawn->GetActorLocation()).GetSafeNormal(), ControlledPawn->GetVelocity().GetSafeNormal()) > 0.5f)
			{
				PreferredOrthoWaypoint = ShorterLegFirstWaypoint;
				AlternativeOrthoWaypoint = LongerLegFirstWaypoint;
			}
			
			// Prefer the waypoint that is in the direction we're already moving.
			if (!NavSys->NavigationRaycast(this, PreferredOrthoWaypoint, NavWaypoint.GetValue(), PotentialHitPoint) || FVector::Dist2D(PotentialHitPoint, TargetLocation) < 400.0f)
			{	
				NewOrthoWaypoint = PreferredOrthoWaypoint;
			}
			else if (!NavSys->NavigationRaycast(this, AlternativeOrthoWaypoint, NavWaypoint.GetValue(), PotentialHitPoint) || FVector::Dist2D(PotentialHitPoint, TargetLocation) < 400.0f)
			{
				NewOrthoWaypoint = AlternativeOrthoWaypoint;
			}
			else
			{
				NewOrthoWaypoint = GetOrthoWaypoint(PotentialHitPoint);
				UE_LOG(LogTemp, Warning, TEXT("AIController: Both orthogonal waypoints are blocked, using recursive call with hit point."));
				DrawDebugSphere(this->GetWorld(), PotentialHitPoint, 50.0f, 1.0, FColor::Purple, false, 10.0f);
				// Recursive call with the hit point as the new target to find a navigable point
			}
		}
		DrawDebugSphere(this->GetWorld(),NewOrthoWaypoint, 50.0f, 1.0, FColor::Green, false, 10.0f);
		return NewOrthoWaypoint;
	}
	else
	{
		return TargetLocation;
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

		// 2. If valid target, move to it (Only if we aren't already!)
		if (CurrentTargetActor != BestTarget)
		{
			CurrentTargetActor = BestTarget;
			NavWaypoint = GetNavWaypoint(BestTarget->GetActorLocation());
			OrthoNavWaypoint = GetOrthoWaypoint(BestTarget->GetActorLocation());
			MoveToLocation(OrthoNavWaypoint.GetValue(), ACCEPTANCE_RADIUS);
			UE_LOG(LogTemp, Log, TEXT("AI: Found new target %s, moving."), *BestTarget->GetName());
		}
		else
		{
			NavWaypoint = GetNavWaypoint(BestTarget->GetActorLocation());
			OrthoNavWaypoint = GetOrthoWaypoint(BestTarget->GetActorLocation());
			MoveToLocation(OrthoNavWaypoint.GetValue(), ACCEPTANCE_RADIUS);
		}
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