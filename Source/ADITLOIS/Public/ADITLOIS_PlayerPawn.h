// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "ADITLOIS_PlayerPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AActor;
class APawn;
class APlayerState;
class UPawnMovementComponent;
class UFloatingPawnMovement;
class USkeletalMeshComponent;
class UBoxComponent;
struct FHitResult;

UCLASS()
class ADITLOIS_API AADITLOIS_PlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AADITLOIS_PlayerPawn();

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

	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	/** Movement component used for movement logic in various movement modes (walking, falling, etc), containing relevant settings and functions to control movement. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	/** The BoxComponent being used for movement collision (by FloatingPawnMovement). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> springArm = nullptr;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> camera = nullptr;

	UPROPERTY()
	FHitResult hitResult;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> interactionTarget = nullptr;

	UPROPERTY()
	FVector startPoint;

	UPROPERTY()
	FRotator viewRotation;

	UPROPERTY()
	FVector endPoint;

	UFUNCTION(Server, Reliable)
	virtual void ServerSetInteractionTarget(bool bHit, FHitResult localHitResult);
};
