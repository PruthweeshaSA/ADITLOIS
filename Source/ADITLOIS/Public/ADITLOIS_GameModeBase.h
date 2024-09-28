// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ADITLOIS_GameModeBase.generated.h"

/**
 *
 */

class AAIController;

UCLASS()
class ADITLOIS_API AADITLOIS_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
};
