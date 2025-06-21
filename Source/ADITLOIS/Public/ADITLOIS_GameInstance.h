#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BlueprintDataDefinitions.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "ADITLOIS_GameInstance.generated.h"

class UCreateSessionCallbackProxyAdvanced;
class UWorld;

UCLASS()
class ADITLOIS_API UADITLOIS_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    UFUNCTION(BlueprintCallable)
    void HostGameSession(FName SessionName, int32 MaxPlayers);

    UPROPERTY()
    UCreateSessionCallbackProxyAdvanced* StoredProxy;

    UFUNCTION()
    void OnSessionCreatedSuccess();

    UFUNCTION()
    void OnSessionCreatedFailure();
};
