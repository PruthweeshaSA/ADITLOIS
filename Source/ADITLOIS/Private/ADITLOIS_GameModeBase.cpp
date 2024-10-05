// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_GameModeBase.h"
#include "ADITLOIS_SaveGame.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AADITLOIS_GameModeBase::AADITLOIS_GameModeBase()
{
    static ConstructorHelpers::FClassFinder<ACharacter> spawnClassFinder(TEXT("'/Game/Blueprints/Character_Blueprints/BP_ADITLOIS_PlayerCharacter'"));
    characterClass = spawnClassFinder.Succeeded() ? spawnClassFinder.Class : nullptr;
    if (characterClass)
    {
        UE_LOG(LogTemp, Log, TEXT("characterClass found: %s"), *characterClass->GetName());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(0, 192, 128), characterClass->GetName());
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("characterClass found!"));
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(255, 64, 64), TEXT("characterClass not found!!!!!!!!!"));
        }
    }
}

void AADITLOIS_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle TimerHandle;
    float DelayTime = 5.0f; // Delay time in seconds

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                                           {
        // Code to execute after the delay
        int32 numHumanPlayers = GetNumPlayers();

        TArray<AAIController*> bots;

        while (bots.Num() + numHumanPlayers < 8)
        {
            FVector spawnLocation = FVector(0.0f);
            FRotator spawnRotator = FRotator::ZeroRotator;
            TObjectPtr<AAIController> botAdded = Cast<AAIController>(GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(), spawnLocation, spawnRotator));
            bots.Add(botAdded);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(0, 192, 128), FString::Printf(TEXT("%d"), bots.Num()));
            }
            spawnLocation = FindPlayerStart(botAdded)->GetActorLocation();
            spawnRotator = FindPlayerStart(botAdded)->GetActorRotation();
            TObjectPtr<AADITLOIS_PlayerCharacter> botCharacterAdded = Cast<AADITLOIS_PlayerCharacter>(GetWorld()->SpawnActor<AADITLOIS_PlayerCharacter>(characterClass, spawnLocation, spawnRotator));
            botAdded->Possess(botCharacterAdded);
            if (GEngine && botCharacterAdded && characterClass)
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(32, 32, 32), FString::Printf(TEXT("%d"), botCharacterAdded->GetActorLocation().Z));
            }

        } }, DelayTime, false);
}

void AADITLOIS_GameModeBase::SaveGame(AADITLOIS_PlayerController *pController)
{
    UADITLOIS_SaveGame *SavedGameInstance = Cast<UADITLOIS_SaveGame>(UGameplayStatics::CreateSaveGameObject(UADITLOIS_SaveGame::StaticClass()));

    if (SavedGameInstance)
    {
        SavedGameInstance->playerName = pController->Player->GetName();
        SavedGameInstance->playerTransform = pController->GetPawn()->GetActorTransform();

        UGameplayStatics::SaveGameToSlot(SavedGameInstance, TEXT("SaveGameSlot"), 0);
    }
}

void AADITLOIS_GameModeBase::LoadGame(AADITLOIS_PlayerController *pController)
{
    UADITLOIS_SaveGame *SavedGameInstance = Cast<UADITLOIS_SaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SaveGameSlot"), 0));

    if (SavedGameInstance)
    {
        FString playerName = pController->Player->GetName();

        if (SavedGameInstance->playerName == playerName)
        {
            pController->GetPawn()->SetActorTransform(SavedGameInstance->playerTransform);
        }
    }
}
