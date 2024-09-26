// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_PlayerCharacter.h"

// Sets default values
AADITLOIS_PlayerCharacter::AADITLOIS_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AADITLOIS_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
// void AADITLOIS_PlayerCharacter::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);

// }

// Called to bind functionality to input
void AADITLOIS_PlayerCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
