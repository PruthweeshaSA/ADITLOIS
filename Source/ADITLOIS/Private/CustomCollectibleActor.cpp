// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCollectibleActor.h"
#include "IADITLOIS_Interactable_Interface.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

// Sets default values
ACustomCollectibleActor::ACustomCollectibleActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	boxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Static Mesh"));
	RootComponent = boxMesh;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACustomCollectibleActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACustomCollectibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACustomCollectibleActor::Interact_Implementation(AActor *Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Custom Collectible Interacted by %s"), *GetNameSafe(Interactor));
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Yellow, FString::Printf(TEXT("You have interacted with %s"), *GetName()));
	}
}
