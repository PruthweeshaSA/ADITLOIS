// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_CustomCollectibleActor.h"
#include "IADITLOIS_Interactable_Interface.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "ADITLOIS_GameState.h"
#include "ADITLOIS_PlayerController.h"
#include "ADITLOIS_GameModeBase.h"
#include "NavigationSystem.h"

// Sets default values
AADITLOIS_CustomCollectibleActor::AADITLOIS_CustomCollectibleActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	boxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root Static Mesh"));
	RootComponent = boxMesh;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AADITLOIS_CustomCollectibleActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AADITLOIS_CustomCollectibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AADITLOIS_CustomCollectibleActor::Interact_Implementation(AActor *Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Custom Collectible Interacted by %s"), *GetNameSafe(Interactor));
	UE_LOG(LogTemp, Warning, TEXT("You have interacted with %s"), *GetName());
	
	// increment score through GameState
	APawn* InteractorPawn = Cast<APawn>(Interactor);
	if (AADITLOIS_GameModeBase* GM = Cast<AADITLOIS_GameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (InteractorPawn && InteractorPawn->GetController() && InteractorPawn->GetController()->IsA(AADITLOIS_PlayerController::StaticClass()))
		{
			GM->AddScore(1);
		}
		else
		{
			GM->AddScore(-1);
		}

		if (UWorld* World = GetWorld())
		{
			if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
			{
				FNavLocation RandomLocation;
				if (NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 2000.0f, RandomLocation))
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					RandomLocation.Location.Z += 160.0f; // slightly above ground
					World->SpawnActor<AADITLOIS_CustomCollectibleActor>(GetClass(), RandomLocation.Location, FRotator::ZeroRotator, SpawnParams);
				}
			}
		}
		Destroy();
	}
	
}
