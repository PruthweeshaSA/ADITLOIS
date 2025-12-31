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
				bool bSpawnPointFound = false;
				int Attempts = 0;
				const int MaxAttempts = 20; // 100 is overkill for a 36% success area

				// Try up to MaxAttempts times to find a point
				while (Attempts < MaxAttempts && !bSpawnPointFound)
				{
					// 1. Get a random point within 50m
					if (NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 5000.0f, RandomLocation))
					{
						// 2. Check if it is OUTSIDE the 30m inner radius (The "Donut" hole)
						if (FVector::DistSquared(GetActorLocation(), RandomLocation.Location) > (3000.0f * 3000.0f))
						{
							bSpawnPointFound = true;
						}
					}
					Attempts++;
				}

				if (bSpawnPointFound)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					
					// Adjust Z slightly
					RandomLocation.Location.Z += 160.0f; 

					World->SpawnActor<AADITLOIS_CustomCollectibleActor>(GetClass(), RandomLocation.Location, FRotator::ZeroRotator, SpawnParams);
				}
			}
		}
		Destroy();
	}
	
}
