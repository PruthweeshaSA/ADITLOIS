// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerCard_Widget.h"
#include "ADITLOIS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "JoinSessionCallbackProxy.h"

void UServerCard_Widget::JoinGameSession(const FBlueprintSessionResult &SessionResult)
{
    if (UWorld *World = GetWorld())
    {
        if (UADITLOIS_GameInstance *GI = World->GetGameInstance<UADITLOIS_GameInstance>())
        {
            // GI->JoinGameSession(SessionResult);
            // UJoinSessionCallbackProxy::JoinSession(GetWorld(), GetOwningPlayer(), SessionResult);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("GameInstance is not of type UADITLOIS_GameInstance."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GetWorld() returned nullptr."));
    }
}
