// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "ADITLOIS_PlayerPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AADITLOIS_PlayerPawn::AADITLOIS_PlayerPawn()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.0f;
    this->bReplicates = true;
    this->SetReplicateMovement(true);

    this->hitResult = FHitResult();

    this->bUseControllerRotationYaw = false;

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshFinder(TEXT("SkeletalMesh'/Game/Assets/SkeletalMeshes/SKM_Sherni.SKM_Sherni'"));

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    BoxComponent->InitBoxExtent(FVector(70.0f, 24.0f, 54.0f));
    BoxComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

    BoxComponent->CanCharacterStepUpOn = ECB_No;
    BoxComponent->SetShouldUpdatePhysicsVolume(true);
    BoxComponent->SetCanEverAffectNavigation(false);
    BoxComponent->bDynamicObstacle = true;
    this->RootComponent = BoxComponent;

    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
    FloatingPawnMovement->UpdatedComponent = BoxComponent;

    this->GetMovementComponent()->SetIsReplicated(true);
    this->GetRootComponent()->SetIsReplicated(true);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);

    if (skeletalMeshFinder.Succeeded())
    {
        SkeletalMesh->SetSkeletalMeshAsset(skeletalMeshFinder.Object);
        SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));
        SkeletalMesh->SetIsReplicated(true);
    }
    springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

    springArm->SetupAttachment(RootComponent);
    camera->SetupAttachment(springArm);
    springArm->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
    springArm->bUsePawnControlRotation = true;

    Cast<UFloatingPawnMovement>(this->GetMovementComponent())->MaxSpeed = 300.0;
}

// Called when the game starts or when spawned
void AADITLOIS_PlayerPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AADITLOIS_PlayerPawn::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    if (HasAuthority())
    {
        SetReplicateMovement(true);
        bReplicates = true;
    }
}

// Called every frame
void AADITLOIS_PlayerPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TObjectPtr<APlayerController> playerController = Cast<APlayerController>(GetController());

    if (playerController)
    {
        playerController->GetPlayerViewPoint(startPoint, viewRotation);
    }

    startPoint = startPoint + viewRotation.Vector() * (springArm->TargetArmLength);
    endPoint = startPoint + viewRotation.Vector() * (500.0f);

    FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(this->hitResult, startPoint, endPoint, ECC_Visibility, TraceParams);

    TObjectPtr<APlayerState> playerState = playerController ? playerController->PlayerState : nullptr;

    if (HasAuthority())
    {
        interactionTarget = bHit ? this->hitResult.GetActor() : nullptr;
    }
    else
    {
        FHitResult localHitResult = this->hitResult;
        ServerSetInteractionTarget(bHit, localHitResult);
        // interactionTarget = bHit ? this->hitResult.GetActor() : nullptr;
    }

    if (GEngine && playerState)
    {
        int32 playerId = playerState->GetPlayerId();
        FString hitDebugMessage = interactionTarget ? interactionTarget->GetName() : FString::Printf(TEXT("NullPtr"));
        GEngine->AddOnScreenDebugMessage(playerId, 1.0f, FColor(0, 192, 64), FString::Printf(TEXT("Interaction Target: %s"), *hitDebugMessage));
    }
}

void AADITLOIS_PlayerPawn::ServerSetInteractionTarget_Implementation(bool bHit, FHitResult localHitResult)
{
    this->interactionTarget = bHit ? localHitResult.GetActor() : nullptr;
}

// Called to bind functionality to input
void AADITLOIS_PlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AADITLOIS_PlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AADITLOIS_PlayerPawn, interactionTarget);
    DOREPLIFETIME(AADITLOIS_PlayerPawn, springArm);
    DOREPLIFETIME(AADITLOIS_PlayerPawn, camera);
}
