// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "ADITLOIS_PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AADITLOIS_PlayerCharacter::AADITLOIS_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.25f;
	this->bReplicates = true;
	this->SetReplicateMovement(true);
	this->GetMovementComponent()->SetIsReplicated(true);
	this->GetCapsuleComponent()->SetIsReplicated(true);
	this->GetMesh()->SetIsReplicated(true);
	this->hitResult = FHitResult();

	this->bUseControllerRotationYaw = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshFinder(TEXT("SkeletalMesh'/Game/Assets/SkeletalMeshes/SKM_Sherni.SKM_Sherni'"));

	if (skeletalMeshFinder.Succeeded())
	{
		this->GetMesh()->SetSkeletalMeshAsset(skeletalMeshFinder.Object);
		this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));
	}
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);
	springArm->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
	springArm->bUsePawnControlRotation = true;

	Cast<UCharacterMovementComponent>(this->GetMovementComponent())->MaxWalkSpeed = 300.0;
}

// Called when the game starts or when spawned
void AADITLOIS_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AADITLOIS_PlayerCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		SetReplicateMovement(true);
		bReplicates = true;
	}
}

// Called every frame
void AADITLOIS_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TObjectPtr<APlayerController> playerController = Cast<APlayerController>(GetController());

	// startPoint = this->GetActorLocation() + (springArm ? springArm->GetRelativeLocation() : FVector(0.0f));
	// startPoint = startPoint + (camera ? camera->GetRelativeLocation() : FVector(0.0f));
	// viewRotation = playerController ? playerController->GetControlRotation() : FRotator(0.0);
	// endPoint = startPoint + (viewRotation.Vector()) * (springArm ? springArm->TargetArmLength + 100.0f : 400.0f);

	if (playerController)
	{
		playerController->GetPlayerViewPoint(startPoint, viewRotation);
	}

	startPoint = startPoint + viewRotation.Vector() * (springArm->TargetArmLength);
	endPoint = startPoint + viewRotation.Vector() * (100.0f);

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(this->hitResult, startPoint, endPoint, ECC_Visibility, TraceParams);

	TObjectPtr<APlayerState> playerState = playerController ? playerController->PlayerState : nullptr;

	// if (GEngine && playerState)
	// {
	// 	int32 playerId = playerState->GetPlayerId();
	// 	FString formattedStartVector = FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"), startPoint.X, startPoint.Y, startPoint.Z);
	// 	FString formattedEndVector = FString::Printf(TEXT("X: %.2f Y: %.2f Z: %.2f"), endPoint.X, endPoint.Y, endPoint.Z);
	// 	GEngine->AddOnScreenDebugMessage(playerId, 2.0f, FColor(32, 64, 128), FString::Printf(TEXT("START: %s ----> END: %s"), *formattedStartVector, *formattedEndVector));
	// }

	if (HasAuthority())
	{
		interactionTarget = bHit ? this->hitResult.GetActor() : nullptr;
	}
	else
	{
		ServerSetInteractionTarget(bHit);
		// interactionTarget = bHit ? this->hitResult.GetActor() : nullptr;
	}

	if (GEngine && playerState)
	{
		int32 playerId = playerState->GetPlayerId();
		FString hitDebugMessage = interactionTarget ? interactionTarget->GetName() : FString::Printf(TEXT("NullPtr"));
		GEngine->AddOnScreenDebugMessage(playerId, 1.0f, FColor(0, 192, 64), FString::Printf(TEXT("Interaction Target: %s"), *hitDebugMessage));
	}
}

void AADITLOIS_PlayerCharacter::ServerSetInteractionTarget_Implementation(bool bHit)
{
	this->interactionTarget = bHit ? this->hitResult.GetActor() : nullptr;
}

// Called to bind functionality to input
void AADITLOIS_PlayerCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AADITLOIS_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AADITLOIS_PlayerCharacter, interactionTarget);
	DOREPLIFETIME(AADITLOIS_PlayerCharacter, springArm);
	DOREPLIFETIME(AADITLOIS_PlayerCharacter, camera);
}
