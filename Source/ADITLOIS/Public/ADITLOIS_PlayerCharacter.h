// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "ADITLOIS_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AActor;
class ACharacter;
class APlayerState;
class UPawnMovementComponent;
class UCharacterMovementComponent;
class UCapsuleComponent;

UCLASS()
class ADITLOIS_API AADITLOIS_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AADITLOIS_PlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController *NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(class TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> springArm = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> camera = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> interactionTarget = nullptr;
};
