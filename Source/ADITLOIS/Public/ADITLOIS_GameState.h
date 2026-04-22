#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ADITLOIS_GameState.generated.h"

/**
 * Custom GameState to handle global game variables like score.
 */
UCLASS()
class ADITLOIS_API AADITLOIS_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AADITLOIS_GameState();

public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Score")
	int32 GlobalScore;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "MovementConstraint")
	bool bIsMovementConstrained;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "PoleAngle")
	float fPoleAngle;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddToGlobalScore(int32 Points);

	UFUNCTION(BlueprintCallable, Category = "Score")
	void ResetGlobalScore();

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetGlobalScore() const;

	UFUNCTION(BlueprintCallable, Category = "PoleAngle")
	float GetPoleAngle();

	UFUNCTION(BlueprintCallable, Category = "MovementConstraint")
	bool GetIsMovementConstrained();
};