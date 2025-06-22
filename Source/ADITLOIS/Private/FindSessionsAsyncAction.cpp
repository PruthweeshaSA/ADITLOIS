#include "FindSessionsAsyncAction.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

UFindSessionsAsyncAction *UFindSessionsAsyncAction::FindGameSessions(APlayerController *PlayerController, int32 InMaxResults)
{
    UFindSessionsAsyncAction *Node = NewObject<UFindSessionsAsyncAction>();
    Node->PlayerController = PlayerController;
    Node->MaxResults = InMaxResults;
    return Node;
}

void UFindSessionsAsyncAction::Activate()
{
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is nullptr."));

        OnFailure.Broadcast();
        SetReadyToDestroy();
        return;
    }

    TArray<FSessionsSearchSetting> EmptySearchSettings;

    Proxy = UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(
        PlayerController,
        PlayerController,
        MaxResults,
        false, // bUseLAN
        EBPServerPresenceSearchType::AllServers,
        EmptySearchSettings,
        false, // Empty Servers Only
        false, // NonEmpty Servers Only
        false, // Secure Servers Only
        true,  // Search Lobbies
        0      // Min slots available
    );

    UE_LOG(LogTemp, Error, TEXT("UFindSessionsAsyncAction::Activate invoked."));

    if (!Proxy)
    {
        OnFailure.Broadcast();
        SetReadyToDestroy();
        UE_LOG(LogTemp, Error, TEXT("FindSessionsCallbackProxyAdvanced is nullptr."));

        return;
    }

    Proxy->OnSuccess.AddDynamic(this, &UFindSessionsAsyncAction::HandleSuccess);
    Proxy->OnFailure.AddDynamic(this, &UFindSessionsAsyncAction::HandleFailure);

    UE_LOG(LogTemp, Error, TEXT("UFindSessionsAsyncAction::Delegates assigned."));

    Proxy->Activate();
}

void UFindSessionsAsyncAction::HandleSuccess(const TArray<FBlueprintSessionResult> &Results)
{
    UE_LOG(LogTemp, Error, TEXT("OnSessionsFound: Found %d sessions."), Results.Num());
    OnSuccess.Broadcast(Results);
    SetReadyToDestroy();
}

void UFindSessionsAsyncAction::HandleFailure(const TArray<FBlueprintSessionResult> &Results)
{
    UE_LOG(LogTemp, Error, TEXT("Failure: could not find sessions."));
    OnFailure.Broadcast();
    SetReadyToDestroy();
}
