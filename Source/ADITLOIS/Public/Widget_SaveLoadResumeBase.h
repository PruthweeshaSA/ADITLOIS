// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_SaveLoadResumeBase.generated.h"

class AADITLOIS_PlayerController;

/**
 *
 */
UCLASS(Blueprintable)
class ADITLOIS_API UWidget_SaveLoadResumeBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidget_SaveLoadResumeBase(const FObjectInitializer &ObjectInitializer);

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
