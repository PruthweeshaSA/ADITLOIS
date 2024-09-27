// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AADITLOIS_PlayerCharacter::AADITLOIS_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = true;
	SetReplicateMovement(true);

	this->bUseControllerRotationYaw = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshFinder(TEXT("SkeletalMesh'/Game/SkeletalMeshes/SKM_Sherni.SKM_Sherni'"));

	if (skeletalMeshFinder.Succeeded())
	{
		this->GetMesh()->SetSkeletalMeshAsset(skeletalMeshFinder.Object);
		this->GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));
	}
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	springArm->SetupAttachment(RootComponent);
	camera->SetupAttachment(springArm);
	springArm->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
	springArm->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AADITLOIS_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AADITLOIS_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Hit;

	FVector start = camera->GetRelativeLocation() + this->GetActorLocation();
	FVector end = start + (this->GetActorForwardVector() + camera->GetRelativeRotation().Vector()) * 400.0f;
}

// Called to bind functionality to input
void AADITLOIS_PlayerCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
