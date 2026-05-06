// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ADITLOIS_PlayerController.h"
#include "ADITLOIS_PlayerCharacter.h"
#include "ADITLOIS_BotCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "ADITLOIS_AIController.h"
#include "GameFramework/HUD.h"
#include "ADITLOIS_GameModeBase.generated.h"

/**
 *
 */

class AAIController;
class AADITLOIS_PlayerCharacter;
class AADITLOIS_PlayerController;
class UGameplayStatics;
class AADITLOIS_AIcontroller;
class UADITLOIS_SaveGame;
class UADITLOIS_GameState;


UCLASS()
class ADITLOIS_API AADITLOIS_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AADITLOIS_GameModeBase();

	virtual void BeginPlay() override;

	virtual void SaveGame(AADITLOIS_PlayerController *pController);

	virtual void LoadGame(AADITLOIS_PlayerController *pController);

	UFUNCTION(Blueprintable)
	virtual void SpawnBots();

	void AddScore(int32 ScoreToAdd);

	UPROPERTY()
	TSubclassOf<ACharacter> playerCharacterClass = nullptr;

	UPROPERTY()
	TSubclassOf<ACharacter> botCharacterClass = nullptr;

	UPROPERTY()
	TSubclassOf<APlayerController> controllerClass = nullptr;

	UPROPERTY()
	TSubclassOf<AHUD> hudBlueprintClass = nullptr;

private:
	int32 getTotalAIControllers();
};
