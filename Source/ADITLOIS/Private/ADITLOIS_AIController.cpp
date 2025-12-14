#include "ADITLOIS_AIController.h"
#include "EngineUtils.h" // For TActorIterator
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "IADITLOIS_Interactable_Interface.h" // Include your interface header
#include "NavigationSystem.h"
// Note: Replace "ADITLOIS_Interactable_Interface.h" with your actual path/filename

AADITLOIS_AIController::AADITLOIS_AIController()
{
	// Set this controller to call Tick() every frame. 
	// Not strictly needed for a timer-based check, but good for debugging.
	PrimaryActorTick.bCanEverTick = false;
}

void AADITLOIS_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AI Controller BeginPlay called."));
	}
}

void AADITLOIS_AIController::OnPossess(APawn *aPawn)
{
	Super::OnPossess(aPawn);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AI Controller OnPossess called."));
	}

	// Start the periodic scanning function
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ScanTimerHandle,
			this,
			&AADITLOIS_AIController::ScanForInteractables,
			ScanInterval, // Time between calls
			true          // Loop/repeat
		);
	}

}

float ACCEPTANCE_RADIUS = 300.0f;


void AADITLOIS_AIController::ScanForInteractables()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		// No pawn to control, stop scanning.
		GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AIcontroller: No pawn to control, stop scanning."));
		}
		return;
	}

	AActor* ClosestTarget = nullptr;
	float ClosestDistanceSq = MAX_FLT;
	const FVector CurrentLocation = ControlledPawn->GetActorLocation();

	// Iterate through all actors in the world
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* CurrentActor = *It;

		// 1. Check if the actor implements the target interface
		if (CurrentActor && CurrentActor != ControlledPawn && CurrentActor->Implements<UADITLOIS_Interactable_Interface>())
		{
			// 2. Check distance
			const float DistanceSq = FVector::DistSquared(CurrentLocation, CurrentActor->GetActorLocation());

			// 3. Check if within the scan radius
			if (DistanceSq <= (ScanRadius * ScanRadius))
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AIcontroller: Target found within scan radius"));
				}
			
				// 4. Check if it's the closest one so far
				if (DistanceSq < ClosestDistanceSq)
				{
					ClosestDistanceSq = DistanceSq;
					ClosestTarget = CurrentActor;
					
				}
			}
		}
	}

	// If a target was found, move the pawn towards it
	if (ClosestTarget)
	{
		// Stop scanning while we pursue the target
		GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
		
		// Move the controlled pawn to the target actor's location
		// The MoveToActor function handles pathfinding.
		MoveToActor(ClosestTarget, 
					ACCEPTANCE_RADIUS, // Acceptance Radius (how close the AI needs to get)
					true,   // bStopOnOverlap (optional)
					true);  // bCanStrafe (optional)

		UE_LOG(LogTemp, Log, TEXT("AIController: Found and moving towards Interactable: %s"), *ClosestTarget->GetName());
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AIController: Found and moving towards Interactable"));
		}
	}
	else
	{
		// Move to random location to explore
		FNavLocation RandomLocation;
		UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(CurrentLocation, ScanRadius, RandomLocation))
		{
			MoveToLocation(RandomLocation.Location);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AIController: Moving to random location."));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("AIController: No interactable targets found in range."));
	}
}

// If move completed, restart scanning
void AADITLOIS_AIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	// Movement has completed, restart scanning for interactables
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ScanTimerHandle,
			this,
			&AADITLOIS_AIController::ScanForInteractables,
			ScanInterval, // Time between calls
			true          // Loop/repeat
		);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("AIController: Movement completed, restarting scan."));
	}
}
