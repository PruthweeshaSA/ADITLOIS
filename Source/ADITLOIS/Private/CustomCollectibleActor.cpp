// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCollectibleActor.h"

// Sets default values
ACustomCollectibleActor::ACustomCollectibleActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	boxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Static Mesh"));
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
