// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "ADITLOIS_PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "ADITLOIS_PlayerController.generated.h"

class UEnhancedInputComponent;
class AADITLOIS_PlayerCharacter;
class UInputMappingContext;
class UInputAction;
class UCharacterMovementComponent;
class AADITLOIS_GameModeBase;

/**
 *
 */
UCLASS()
class ADITLOIS_API AADITLOIS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AADITLOIS_PlayerController();

private:
	UFUNCTION(Blueprintable)
	virtual void OnPossess(APawn *aPawn) override;

	UFUNCTION(Blueprintable)
	virtual void SetupInputComponent() override;

	UFUNCTION(Blueprintable)
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UEnhancedInputComponent> enhancedInputComponent = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> inputMappingContext = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionLook = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionMove = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionJump = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionSprint = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionInteract = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionCameraZoom = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionSaveGame = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionLoadGame = nullptr;

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<AADITLOIS_PlayerCharacter> playerCharacter = nullptr;

private:
	UFUNCTION(Server, Reliable)
	void ServerOnPossess(APawn *aPawn);

	UFUNCTION(Blueprintable)
	void OnActionLook(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionMove(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionJump(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionSprintPress(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionSprintRelease(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionInteract(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionCameraZoom(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionSaveGame(const FInputActionValue &Value);

	UFUNCTION(Blueprintable)
	void OnActionLoadGame(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionLook(const FInputActionValue &Value, FRotator actorRotation);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionMove(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionJump(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSprintPress(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSprintRelease(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionInteract(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionCameraZoom(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSaveGame(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionLoadGame(const FInputActionValue &Value);
};
