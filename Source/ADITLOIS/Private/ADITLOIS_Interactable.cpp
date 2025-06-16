#include "ADITLOIS_Interactable.h"

void AADITLOIS_Interactable::Interact_Implementation(AActor *Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("Interacted by %s"), *GetNameSafe(Interactor));
}
