// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ADITLOIS_SaveGame.generated.h"

/**
 *
 */
UCLASS()
class ADITLOIS_API UADITLOIS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UADITLOIS_SaveGame();

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FString playerName;

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FTransform playerTransform;
};
