// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_GameModeBase.h"
#include "ADITLOIS_SaveGame.h"
#include "GameFramework/PlayerState.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "ADITLOIS_AIController.h"
#include "ADITLOIS_GameState.h"

AADITLOIS_GameModeBase::AADITLOIS_GameModeBase()
{
    static ConstructorHelpers::FClassFinder<ACharacter> spawnClassFinder(TEXT("'/Game/Blueprints/Character_Blueprints/BP_ADITLOIS_PlayerCharacter'"));
    characterClass = spawnClassFinder.Succeeded() ? spawnClassFinder.Class : nullptr;
    if (characterClass)
    {
        DefaultPawnClass = characterClass;
        UE_LOG(LogTemp, Log, TEXT("characterClass BluePrint found: %s"), *characterClass->GetName());
    }
    else
    {
        DefaultPawnClass = AADITLOIS_PlayerCharacter::StaticClass();
        UE_LOG(LogTemp, Error, TEXT("characterClass BluePrint not found."));
    }

    static ConstructorHelpers::FClassFinder<APlayerController> controllerClassFinder(TEXT("'/Game/Blueprints/PlayerController_Blueprints/BP_ADITLOIS_PlayerController'"));
    controllerClass = controllerClassFinder.Succeeded() ? controllerClassFinder.Class : nullptr;
    if (controllerClass)
    {
        PlayerControllerClass = controllerClass;
        UE_LOG(LogTemp, Log, TEXT("playerControllerClass BluePrint found: %s"), *controllerClass->GetName());
    }
    else
    {
        PlayerControllerClass = AADITLOIS_PlayerController::StaticClass();
        UE_LOG(LogTemp, Error, TEXT("playerControllerClass BluePrint not found."));
    }

    static ConstructorHelpers::FClassFinder<AHUD> hudClassFinder(TEXT("'/Game/Blueprints/Hud_Blueprints/BP_ADITLOIS_HUD'"));
    hudBlueprintClass = hudClassFinder.Succeeded() ? hudClassFinder.Class : nullptr;
    if (hudBlueprintClass)
    {
        HUDClass = hudBlueprintClass;
        UE_LOG(LogTemp, Log, TEXT("hudClass BluePrint found: %s"), *hudBlueprintClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("hudClass BluePrint not found."));
    }

    GameStateClass = AADITLOIS_GameState::StaticClass();
}

void AADITLOIS_GameModeBase::BeginPlay()
{
    Super::BeginPlay();
}

int32 AADITLOIS_GameModeBase::getTotalAIControllers()
{
    TArray<AActor*> AIControllers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIController::StaticClass(), AIControllers);
    // The total count is:
    return AIControllers.Num();
}

void AADITLOIS_GameModeBase::SpawnBots()
{
    // Code to execute to spawn bots
    int32 numHumanPlayers = GetNumPlayers();

    TArray<AAIController *> bots;

    while (getTotalAIControllers() < numHumanPlayers)
    {
        FVector spawnLocation = FVector(0.0f);
        FRotator spawnRotator = FRotator::ZeroRotator;
        TObjectPtr<AAIController> botAdded = Cast<AAIController>(GetWorld()->SpawnActor<AADITLOIS_AIController>(AADITLOIS_AIController::StaticClass(), spawnLocation, spawnRotator));
        bots.Add(botAdded);
        spawnLocation = FindPlayerStart(botAdded)->GetActorLocation();
        spawnRotator = FindPlayerStart(botAdded)->GetActorRotation();
        TObjectPtr<AADITLOIS_PlayerCharacter> botCharacterAdded = Cast<AADITLOIS_PlayerCharacter>(GetWorld()->SpawnActor<AADITLOIS_PlayerCharacter>(characterClass, spawnLocation, spawnRotator));
        botAdded->Possess(botCharacterAdded);
    }
}

void AADITLOIS_GameModeBase::SaveGame(AADITLOIS_PlayerController *pController)
{
    UADITLOIS_SaveGame *savedGameInstance = Cast<UADITLOIS_SaveGame>(UGameplayStatics::CreateSaveGameObject(UADITLOIS_SaveGame::StaticClass()));

    if (savedGameInstance)
    {
        FString playerName = pController->Player->GetName();
        savedGameInstance->playerName = playerName;
        savedGameInstance->playerTransform = pController->GetPawn()->GetActorTransform();
        savedGameInstance->playerScore = pController->playerScore;
        savedGameInstance->controlRotation = pController->GetControlRotation();

        UGameplayStatics::SaveGameToSlot(savedGameInstance, pController->Player->GetName(), 0);
    }
}

void AADITLOIS_GameModeBase::LoadGame(AADITLOIS_PlayerController *pController)
{
    FString playerName = pController->Player->GetName();
    UADITLOIS_SaveGame *savedGameInstance = Cast<UADITLOIS_SaveGame>(UGameplayStatics::LoadGameFromSlot(playerName, 0));

    if (savedGameInstance)
    {
        if (savedGameInstance->playerName == playerName)
        {
            pController->GetPawn()->SetActorTransform(savedGameInstance->playerTransform);
            pController->playerScore = savedGameInstance->playerScore;
            // pController->SetControlRotation(savedGameInstance->controlRotation);
            pController->playerControllerRotation = savedGameInstance->controlRotation;
            TObjectPtr<APlayerState> playerState = pController ? pController->PlayerState : nullptr;
            if (playerState)
            {
                int32 playerId = playerState->GetPlayerId();
                FString hitDebugMessage = FString::Printf(TEXT("Camera Position loaded from savefile"));
                UE_LOG(LogTemp, Log, TEXT("%s"), *hitDebugMessage);
            }
        }
    }
}

void AADITLOIS_GameModeBase::AddScore(int32 ScoreToAdd)
{
	if (AADITLOIS_GameState* GS = GetGameState<AADITLOIS_GameState>())
	{
		GS->AddToGlobalScore(ScoreToAdd);
        UE_LOG(LogTemp, Log, TEXT("The score is now %d"), GS->GetGlobalScore());
	}
}