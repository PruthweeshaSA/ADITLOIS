// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_SaveGame.h"

UADITLOIS_SaveGame::UADITLOIS_SaveGame()
{
    playerId = -1;
    playerTransform = FTransform(FRotator(0.0f), FVector(0.0f), FVector(1.0f));
}
