// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ADITLOIS_PlayerController.h"
#include "ADITLOIS_PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "ADITLOIS_GameModeBase.generated.h"

/**
 *
 */

class AAIController;
class AADITLOIS_PlayerCharacter;
class AADITLOIS_PlayerController;
class UGameplayStatics;

UCLASS()
class ADITLOIS_API AADITLOIS_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AADITLOIS_GameModeBase();

	virtual void BeginPlay() override;

	virtual void SaveGame(AADITLOIS_PlayerController *pController);

	virtual void LoadGame(AADITLOIS_PlayerController *pController);

	UPROPERTY()
	TSubclassOf<ACharacter> characterClass = nullptr;
};
