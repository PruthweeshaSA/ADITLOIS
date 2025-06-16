#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IADITLOIS_Interactable_Interface.h"
#include "ADITLOIS_Interactable.generated.h"

UCLASS()
class ADITLOIS_API AADITLOIS_Interactable : public AActor, public IADITLOIS_Interactable_Interface
{
    GENERATED_BODY()

public:
    virtual void Interact_Implementation(AActor *Interactor) override;
};
