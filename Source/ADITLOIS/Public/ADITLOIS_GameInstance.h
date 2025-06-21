#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BlueprintDataDefinitions.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "ADITLOIS_GameInstance.generated.h"

UCLASS()
class ADITLOIS_API UADITLOIS_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable)
    void HostGameSession(FName SessionName, int32 MaxPlayers);

    UFUNCTION(BlueprintCallable)
    void JoinGameSession(const FBlueprintSessionResult &SessionResult);

    UFUNCTION()
    void OnSessionCreatedSuccess();

    UFUNCTION()
    void OnSessionCreatedFailure();

    

private:
    UCreateSessionCallbackProxyAdvanced *StoredProxy;

    FDelegateHandle JoinSessionCompleteHandle;

    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
