#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "ADITLOIS_AIController.generated.h"

// Forward Declaration for your custom interface
// You must ensure this interface file exists and is included in your build.
class IADITLOIS_Interactable_Interface;

class UPathFollowingComponent;
class UNavigationSystemV1;



/**
 * Barebones AIController that periodically scans for interactable objects and moves towards the closest one.
 */
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

	/** The time interval (in seconds) for scanning for interactable objects. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ScanInterval = 1.0f; // Check every 1 second

	/** The radius used for the sphere trace/overlap check. */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ScanRadius = 10000.0f; // 100-meter radius

	/**
	 * Scans the surrounding area for objects implementing the interactable interface.
	 * Moves the controlled pawn towards the closest one.
	 */
	void ScanForInteractables();

	UFUNCTION(BlueprintCallable)
	virtual void OnPossess(APawn *aPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult &Result) override;
};