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
#include "Net/UnrealNetwork.h"

#include "ADITLOIS_PlayerController.generated.h"

class UEnhancedInputComponent;
class AADITLOIS_PlayerCharacter;
class UInputMappingContext;
class UInputAction;
class UCharacterMovementComponent;
class AADITLOIS_GameModeBase;
class UUserWidget;
class UGameplayStatics;
class UADITLOIS_Interactable_Interface;
class APawn;
class ACharacter;
class UFloatingPawnMovement;

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
	UFUNCTION(BlueprintCallable)
	virtual void OnPossess(APawn *aPawn) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
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

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionSwitchCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> ActionPauseGame = nullptr;

public:
	void GetLifetimeReplicatedProps(class TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AADITLOIS_PlayerCharacter> playerCharacter = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 playerScore = 0;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FRotator playerControllerRotation = FRotator(0.0f);

private:
	UFUNCTION(Server, Reliable)
	void ServerOnPossess(APawn *aPawn);

public:
	UFUNCTION(BlueprintCallable)
	void OnActionLook(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionMove(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionJump(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionSprintPress(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionSprintRelease(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionInteract(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionCameraZoom(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable)
	void OnActionSaveGame();

	UFUNCTION(BlueprintCallable)
	void OnActionLoadGame();

	UFUNCTION(BlueprintCallable)
	void OnActionSwitchCharacter();

	UFUNCTION(BlueprintCallable)
	void OnActionPauseGame();

private:
	UFUNCTION(Server, Unreliable, Blueprintable)
	void ServerOnActionLook(FRotator controllerRotation, FRotator characterRotation);

	UFUNCTION(Server, Unreliable, Blueprintable)
	void ServerOnActionMove(FVector characterLocation, FRotator characterRotation);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSprintPress(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSprintRelease(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionInteract(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionCameraZoom(const FInputActionValue &Value);

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSaveGame();

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionLoadGame();

	UFUNCTION(Server, Reliable, Blueprintable)
	void ServerOnActionSwitchCharacter();
};
