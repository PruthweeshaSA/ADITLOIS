// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_PlayerPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/CollisionProfile.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AADITLOIS_PlayerPawn::AADITLOIS_PlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0f;
    this->bReplicates = true;
	this->SetReplicateMovement(false);
	

    hitResult = FHitResult();
    bUseControllerRotationYaw = false;

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMeshFinder(
        TEXT("SkeletalMesh'/Game/Assets/SkeletalMeshes/SKM_Sherni.SKM_Sherni'"));

    boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    boxComponent->InitBoxExtent(FVector(70.0f, 24.0f, 54.0f));
    boxComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    boxComponent->CanCharacterStepUpOn = ECB_No;
    boxComponent->SetShouldUpdatePhysicsVolume(true);
    boxComponent->SetCanEverAffectNavigation(false);
    boxComponent->bDynamicObstacle = true;
    boxComponent->SetNotifyRigidBodyCollision(true);
    RootComponent = boxComponent;

    FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
    FloatingPawnMovement->UpdatedComponent = boxComponent;

    GetMovementComponent()->SetIsReplicated(true);
    GetRootComponent()->SetIsReplicated(true);

    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    SkeletalMesh->SetupAttachment(RootComponent);

    if (skeletalMeshFinder.Succeeded())
    {
        SkeletalMesh->SetSkeletalMeshAsset(skeletalMeshFinder.Object);
        SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -85.0f));
        // SkeletalMesh->SetIsReplicated(true);
    }

    this->FloatingPawnMovement->SetIsReplicated(true);
	this->GetRootComponent()->SetIsReplicated(true);
	this->SkeletalMesh->SetIsReplicated(true);
	this->hitResult = FHitResult();

    springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));

    springArm->SetupAttachment(RootComponent);
    camera->SetupAttachment(springArm);
    springArm->SocketOffset = FVector(0.0f, 50.0f, 50.0f);
    springArm->bUsePawnControlRotation = true;

    OverlapBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBoxComponent"));
    OverlapBoxComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    OverlapBoxComponent->SetGenerateOverlapEvents(true);
    OverlapBoxComponent->InitBoxExtent(FVector(30.0f, 24.0f, 15.0f));
    OverlapBoxComponent->SetupAttachment(RootComponent);
    OverlapBoxComponent->SetRelativeLocation(FVector(100.0f, 0.0f, 39.0f));

    Cast<UFloatingPawnMovement>(GetMovementComponent())->MaxSpeed = 300.0f;

    this->OverlapBoxComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AADITLOIS_PlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    if (boxComponent)
    {
        boxComponent->OnComponentHit.AddDynamic(this, &AADITLOIS_PlayerPawn::BoxComponent_ComponentHit);
    }
}

void AADITLOIS_PlayerPawn::PossessedBy(AController *NewController)
{
    Super::PossessedBy(NewController);

    if (HasAuthority())
    {
        SetReplicateMovement(false);
        bReplicates = true;
    }
}

void AADITLOIS_PlayerPawn::ConditionalClimbEnable()
{
    TArray<AActor *> ActorsOverlapping;

    if (OverlapBoxComponent == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("OverlapBoxComponent is null"));
        return;
    }
    OverlapBoxComponent->GetOverlappingActors(ActorsOverlapping);

    bCanClimb = (ActorsOverlapping.Num() <= 1);

    UE_LOG(LogTemp, Warning, TEXT("Result of CanClimb: %s"), bCanClimb ? TEXT("true") : TEXT("false"));
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

    float ArmLength = (springArm ? springArm->TargetArmLength : 0.0f);
    startPoint = startPoint + viewRotation.Vector() * ArmLength;
    endPoint = startPoint + viewRotation.Vector() * 500.0f;

    FCollisionQueryParams TraceParams(FName(TEXT("")), false, this);
    bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startPoint, endPoint, ECC_Visibility, TraceParams);

    TObjectPtr<APlayerState> playerState = playerController ? playerController->PlayerState : nullptr;
    AActor *LocalTarget = bHit ? hitResult.GetActor() : nullptr;

    if (HasAuthority())
    {
        interactionTarget = LocalTarget;
    }
    else
    {
        if (LocalTarget != LastSentInteractionTarget)
        {
            LastSentInteractionTarget = LocalTarget;
            ServerSetInteractionTarget(bHit, hitResult.ImpactPoint, LocalTarget);
        }
    }

    if (HasAuthority())
    {
        fServerTransform = GetActorTransform();
    }
    else
    {
        if (fServerTransform.GetLocation() != GetActorLocation() ||
            fServerTransform.GetRotation() != GetActorRotation().Quaternion())
        {
            fServerTransform = GetActorTransform();
            ServerSetActorTransform(fServerTransform);
        }
    }


    if (GEngine && playerState)
    {
        int32 playerId = playerState->GetPlayerId();
        FString hitDebugMessage = interactionTarget ? interactionTarget->GetName() : TEXT("NullPtr");
        GEngine->AddOnScreenDebugMessage(playerId, 1.0f, FColor(0, 192, 64),
                                         FString::Printf(TEXT("Interaction Target: %s"), *hitDebugMessage));
    }
}

void AADITLOIS_PlayerPawn::ServerSetInteractionTarget_Implementation(bool bHit, FVector HitLocation, AActor *HitActor)
{
    interactionTarget = bHit ? HitActor : nullptr;
}

void AADITLOIS_PlayerPawn::AADITLOIS_PlayerPawn::ServerSetActorTransform_Implementation(FTransform NewTransform)
{
    SetActorTransform(NewTransform);
    fServerTransform = NewTransform;
}

void AADITLOIS_PlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AADITLOIS_PlayerPawn::BoxComponent_ComponentHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
                                                     UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
    if (OtherActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit detected with actor: %s"), *OtherActor->GetName());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor(0, 192, 64),
                                             FString::Printf(TEXT("Hit detected with actor: %s"), *OtherActor->GetName()));
        }

        ConditionalClimbEnable();

        if (bCanClimb)
        {
            FVector lift = FVector(0.0f, 0.0f, (Hit.Location.Z - (boxComponent->GetComponentLocation().Z - boxComponent->GetScaledBoxExtent().Z)));
            boxComponent->AddImpulse(lift * (HasAuthority()?100.0f:200.0f));
            // FloatingPawnMovement->AddInputVector(lift*(HasAuthority()?100.0f:150.0f), true);
            
            if (HasAuthority())
            {
                fServerTransform = GetActorTransform();
            }
            else
            {
                if (fServerTransform.GetLocation() != GetActorLocation() ||
                    fServerTransform.GetRotation() != GetActorRotation().Quaternion())
                {
                    fServerTransform = GetActorTransform();
                    ServerSetActorTransform(fServerTransform);
                }
            }
        }
    }
}

void AADITLOIS_PlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AADITLOIS_PlayerPawn, interactionTarget);
	DOREPLIFETIME(AADITLOIS_PlayerPawn, springArm);
	DOREPLIFETIME(AADITLOIS_PlayerPawn, camera);
    DOREPLIFETIME(AADITLOIS_PlayerPawn, SkeletalMesh);
    DOREPLIFETIME(AADITLOIS_PlayerPawn, fServerTransform);
}
