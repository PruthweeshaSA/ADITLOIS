#include "ADITLOIS_GameState.h"
#include "Net/UnrealNetwork.h"

AADITLOIS_GameState::AADITLOIS_GameState()
{
	GlobalScore = 0;
}

void AADITLOIS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AADITLOIS_GameState, GlobalScore);
}