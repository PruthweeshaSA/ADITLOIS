#pragma once

#include "UObject/Interface.h"
#include "IADITLOIS_Interactable_Interface.generated.h"

// This is the UINTERFACE part
UINTERFACE(MinimalAPI, Blueprintable)
class UADITLOIS_Interactable_Interface : public UInterface
{
    GENERATED_BODY()
};

// This is the actual interface definition
class ADITLOIS_API IADITLOIS_Interactable_Interface
{
    GENERATED_BODY()

public:
    // BlueprintNativeEvent allows BP to override, and you can have a default C++ implementation
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void Interact(AActor *Interactor);
};
