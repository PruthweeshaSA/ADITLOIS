// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_GameModeBase.h"
#include "AIController.h"

void AADITLOIS_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle TimerHandle;
    float DelayTime = 5.0f; // Delay time in seconds

    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
                                           {
        // Code to execute after the delay
        int32 numHumanPlayers = GetNumPlayers();

        TArray<AAIController *> bots;

        while (bots.Num() + numHumanPlayers < 8)
        {
            bots.Add(Cast<AAIController>(GetWorld()->SpawnActor(AAIController::StaticClass())));
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(0, 192, 128), FString::Printf(TEXT("%d"), bots.Num()));
            }
        } }, DelayTime, false);
}
