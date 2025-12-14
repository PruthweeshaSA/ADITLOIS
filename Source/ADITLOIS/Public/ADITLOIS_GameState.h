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

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Score")
	int32 GlobalScore;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};