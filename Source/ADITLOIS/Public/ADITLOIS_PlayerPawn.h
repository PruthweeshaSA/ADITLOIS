// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

#include "ADITLOIS_PlayerPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AActor;
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
	AADITLOIS_PlayerPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController *NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	void GetLifetimeReplicatedProps(class TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	UFUNCTION()
	void ConditionalClimbEnable();

	/** Skeletal mesh for the pawn */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	/** Floating pawn movement */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;

	/** Box collision used as root */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> OverlapBoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> springArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> camera;

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

	UPROPERTY()
	bool bCanClimb = false;

	// Tracks last interaction target we sent to the server
	UPROPERTY()
	TObjectPtr<AActor> LastSentInteractionTarget = nullptr;

	UFUNCTION()
	void BoxComponent_ComponentHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
								   UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

	UFUNCTION(Server, Reliable)
	void ServerSetInteractionTarget(bool bHit, FVector HitLocation, AActor *HitActor);
};
