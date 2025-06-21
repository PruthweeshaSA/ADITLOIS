// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintDataDefinitions.h" 
#include "ServerCard_Widget.generated.h"

/**
 *
 */
UCLASS()
class ADITLOIS_API UServerCard_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void JoinGameSession(const FBlueprintSessionResult& SessionResult);
};
