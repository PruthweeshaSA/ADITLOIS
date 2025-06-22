#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BlueprintDataDefinitions.h"
#include "FindSessionsCallbackProxyAdvanced.h"
#include "FindSessionsAsyncAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFindSessionsSuccess, const TArray<FBlueprintSessionResult> &, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFindSessionsFailure);

UCLASS()
class ADITLOIS_API UFindSessionsAsyncAction : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnFindSessionsSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnFindSessionsFailure OnFailure;

    // Static Blueprint-callable function to trigger the async node
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
    static UFindSessionsAsyncAction *FindGameSessions(
        APlayerController *PlayerController,
        int32 MaxResults);

    virtual void Activate() override;

private:
    UPROPERTY()
    APlayerController *PlayerController;

    int32 MaxResults;

    UPROPERTY()
    UFindSessionsCallbackProxyAdvanced *Proxy;

    UFUNCTION()
    void HandleSuccess(const TArray<FBlueprintSessionResult> &Results);

    UFUNCTION()
    void HandleFailure(const TArray<FBlueprintSessionResult> &Results);
};
