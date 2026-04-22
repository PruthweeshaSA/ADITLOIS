#include "ADITLOIS_GameState.h"
#include "Net/UnrealNetwork.h"

AADITLOIS_GameState::AADITLOIS_GameState()
{
	GlobalScore = 0;
	bIsMovementConstrained = true;
	fPoleAngle = 0.0f;
}

void AADITLOIS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AADITLOIS_GameState, GlobalScore);
}

void AADITLOIS_GameState::AddToGlobalScore(int32 Points)
{
	GlobalScore += Points;
}

void AADITLOIS_GameState::ResetGlobalScore()
{
	GlobalScore = 0;
}

int32 AADITLOIS_GameState::GetGlobalScore() const
{
	return GlobalScore;
}

float AADITLOIS_GameState::GetPoleAngle()
{
	return fPoleAngle;
}

bool AADITLOIS_GameState::GetIsMovementConstrained()
{
	return bIsMovementConstrained;
}