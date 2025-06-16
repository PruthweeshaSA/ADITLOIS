// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu_Widget.generated.h"

class AADITLOIS_PlayerController;

/**
 *
 */
UCLASS(Blueprintable)
class ADITLOIS_API UPauseMenu_Widget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPauseMenu_Widget(const FObjectInitializer &ObjectInitializer);

	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	TSubclassOf<APlayerController> controllerClass = nullptr;

public:
	UFUNCTION(BlueprintCallable)
	void OnButtonHitResume();

	UFUNCTION(BlueprintCallable)
	void OnButtonHitSaveGame();

	UFUNCTION(BlueprintCallable)
	void OnButtonHitLoadGame();

	// UFUNCTION(BlueprintCallable)
	// void OnButtonHitMainMenu();
};
