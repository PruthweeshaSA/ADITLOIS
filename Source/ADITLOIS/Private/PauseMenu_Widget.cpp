// Fill out your copyright notice in the Description page of Project Settings.

#include "PauseMenu_Widget.h"

#include "ADITLOIS_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

UPauseMenu_Widget::UPauseMenu_Widget(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<APlayerController> controllerClassFinder(TEXT("'/Game/Blueprints/PlayerController_Blueprints/BP_ADITLOIS_PlayerController'"));
    controllerClass = controllerClassFinder.Succeeded() ? controllerClassFinder.Class : nullptr;
    if (controllerClass)
    {
        UE_LOG(LogTemp, Log, TEXT("playerControllerClass BluePrint found: %s"), *controllerClass->GetName());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(0, 192, 128), controllerClass->GetName());
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("controllerClass found!"));
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(255, 64, 64), TEXT("controllerClass not found!!!!!!!!!"));
        }
    }
}

void UPauseMenu_Widget::NativeConstruct()
{
    Super::NativeConstruct();

    AADITLOIS_PlayerController *MyPC = Cast<AADITLOIS_PlayerController>(GetOwningPlayer());
    UGameplayStatics::SetGamePaused(GetWorld(), true);
    if (MyPC)
    {
        FInputModeUIOnly InputMode;
        MyPC->SetInputMode(InputMode);
        MyPC->bShowMouseCursor = true;
    }

    // Widget setup logic goes here, like binding buttons, etc.
}

void UPauseMenu_Widget::OnButtonHitResume()
{
    UE_LOG(LogTemp, Log, TEXT("Resume called"));
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Resume called"));
    }
    AADITLOIS_PlayerController *MyPC = Cast<AADITLOIS_PlayerController>(GetOwningPlayer());
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    if (MyPC)
    {
        FInputModeGameOnly InputMode;
        MyPC->SetInputMode(InputMode);
        MyPC->bShowMouseCursor = false;
    }
    RemoveFromParent();
}

void UPauseMenu_Widget::OnButtonHitSaveGame()
{
    UE_LOG(LogTemp, Log, TEXT("Save Game called"));
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Save Game called"));
    }
    if (controllerClass)
    {
        UE_LOG(LogTemp, Log, TEXT("playerControllerClass BluePrint found: %s"), *controllerClass->GetName());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(0, 192, 128), controllerClass->GetName());
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("controllerClass found!"));
        }

        AADITLOIS_PlayerController *MyPC = Cast<AADITLOIS_PlayerController>(GetOwningPlayer());
        if (MyPC)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Save Game called via native ControllerClass"));
            }
            MyPC->OnActionSaveGame();
        }
    }
}

void UPauseMenu_Widget::OnButtonHitLoadGame()
{
    UE_LOG(LogTemp, Log, TEXT("Save Game called"));
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Load Game called"));
    }
    if (controllerClass)
    {
        UE_LOG(LogTemp, Log, TEXT("playerControllerClass BluePrint found: %s"), *controllerClass->GetName());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(0, 192, 128), controllerClass->GetName());
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("controllerClass found!"));
        }

        AADITLOIS_PlayerController *MyPC = Cast<AADITLOIS_PlayerController>(GetOwningPlayer());
        if (MyPC)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Load Game called via native ControllerClass"));
            MyPC->OnActionLoadGame();
        }
    }
}
