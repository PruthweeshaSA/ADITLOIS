#include "ADITLOIS_QuadrupedPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AADITLOIS_QuadrupedPawn::AADITLOIS_QuadrupedPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    // Root capsule
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    CapsuleComp->InitCapsuleSize(42.f, 88.f);
    CapsuleComp->SetCollisionProfileName("Pawn");
    CapsuleComp->SetIsReplicated(true);
    RootComponent = CapsuleComp;

    // Skeletal mesh
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetIsReplicated(true);

    // Torso collision box
    TorsoBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TorsoBox"));
    TorsoBox->SetupAttachment(RootComponent);
    TorsoBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // overlap only by default
    TorsoBox->SetCollisionResponseToAllChannels(ECR_Overlap);
    TorsoBox->SetGenerateOverlapEvents(true);
    TorsoBox->SetIsReplicated(true);

    // Bind torso events
    TorsoBox->OnComponentBeginOverlap.AddDynamic(this, &AADITLOIS_QuadrupedPawn::OnTorsoOverlap);
    TorsoBox->OnComponentHit.AddDynamic(this, &AADITLOIS_QuadrupedPawn::OnTorsoHit);

    // Spring arm & camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

void AADITLOIS_QuadrupedPawn::BeginPlay()
{
    Super::BeginPlay();
}

void AADITLOIS_QuadrupedPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AADITLOIS_QuadrupedPawn::OnTorsoOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
                                             UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        UE_LOG(LogTemp, Log, TEXT("Torso overlapped with: %s"), *OtherActor->GetName());
    }
}

void AADITLOIS_QuadrupedPawn::OnTorsoHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
                                         UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
    if (OtherActor && OtherActor != this)
    {
        UE_LOG(LogTemp, Log, TEXT("Torso hit: %s"), *OtherActor->GetName());
    }
}
