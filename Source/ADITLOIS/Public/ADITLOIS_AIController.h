#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include <vector> // Required for std::vector
#include "ADITLOIS_GameState.h"

#include "ADITLOIS_AIController.generated.h"

class IADITLOIS_Interactable_Interface;
class UNavigationSystemV1;
class UNavigationPath;
class AADITLOIS_GameState;

UCLASS()
class ADITLOIS_API AADITLOIS_AIController : public AAIController
{
	GENERATED_BODY()

public:
	// Constructor
	AADITLOIS_AIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/** Timer handle for the periodic scan function. */
	FTimerHandle ScanTimerHandle;

	APawn *ControlledPawn;

	TOptional<FVector> TargetLocation;
    TOptional<FVector> OrthoNavWaypoint;

	float ACCEPTANCE_RADIUS = 1.0f;

	double LastScannedTimestamp = 0.0;



	UPROPERTY()
    AActor* CurrentTargetActor;

	/** The time interval (in seconds) for scanning for interactable objects. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ScanInterval = 1.0f; // Check every 1 second

	/** The radius used for the sphere trace/overlap check. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ScanRadius = 10000.0f; // 100-meter radius

	UNavigationSystemV1 *NavSys;

	/**
	 * Scans the surrounding area for objects implementing the interactable interface.
	 * Moves the controlled pawn towards the closest one.
	 */
	void ScanForInteractables();

	UFUNCTION(BlueprintCallable)
	virtual void OnPossess(APawn *aPawn) override;
	
    FVector GetIdealWaypoint();

    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult &Result) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AADITLOIS_GameState> gameState = nullptr;

};