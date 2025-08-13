#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ADITLOIS_QuadrupedPawn.generated.h"

UCLASS()
class ADITLOIS_API AADITLOIS_QuadrupedPawn : public APawn
{
    GENERATED_BODY()

public:
    AADITLOIS_QuadrupedPawn();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Torso collision events
    UFUNCTION()
    void OnTorsoOverlap(UPrimitiveComponent *OverlappedComp, AActor *OtherActor,
                        UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTorsoHit(UPrimitiveComponent *HitComp, AActor *OtherActor,
                    UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit);

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCapsuleComponent *CapsuleComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USkeletalMeshComponent *MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent *TorsoBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent *SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent *Camera;
};
