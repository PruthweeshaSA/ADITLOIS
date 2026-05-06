// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "ADITLOIS_BotCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AADITLOIS_BotCharacter::AADITLOIS_BotCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.05f;
	this->bReplicates = true;
	this->SetReplicateMovement(true);
	this->GetMovementComponent()->SetIsReplicated(true);
	this->GetCapsuleComponent()->SetIsReplicated(true);
	this->GetMesh()->SetIsReplicated(true);
	this->hitResult = FHitResult();

	this->bUseControllerRotationYaw = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshFinder(TEXT("SkeletalMesh'/Game/Assets/SkeletalMeshes/Bot_for_ADITLOIS.Bot_for_ADITLOIS'"));

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
void AADITLOIS_BotCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AADITLOIS_BotCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		SetReplicateMovement(true);
		bReplicates = true;
	}
}

// Called every frame
void AADITLOIS_BotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AADITLOIS_BotCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AADITLOIS_BotCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AADITLOIS_BotCharacter, springArm);
	DOREPLIFETIME(AADITLOIS_BotCharacter, camera);
}
