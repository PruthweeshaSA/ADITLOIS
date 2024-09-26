// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_PlayerController.h"

AADITLOIS_PlayerController::AADITLOIS_PlayerController()
{
    bReplicates = true;

    // Use FObjectFinder to find the InputMappingContext
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextFinder(TEXT("InputMappingContext'/Game/Inputs/IMC_Locomotion.IMC_Locomotion'"));
    if (InputMappingContextFinder.Succeeded())
    {
        inputMappingContext = InputMappingContextFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Mapping Context found: %s"), *inputMappingContext->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Mapping Context"));
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookFinder(TEXT("InputAction'/Game/Inputs/IA_Look.IA_Look'"));
    if (InputActionLookFinder.Succeeded())
    {
        ActionLook = InputActionLookFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Look found: %s"), *ActionLook->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Look"));
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveFinder(TEXT("InputAction'/Game/Inputs/IA_Move.IA_Move'"));
    if (InputActionMoveFinder.Succeeded())
    {
        ActionMove = InputActionMoveFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Move found: %s"), *ActionMove->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Move"));
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpFinder(TEXT("InputAction'/Game/Inputs/IA_Jump.IA_Jump'"));
    if (InputActionJumpFinder.Succeeded())
    {
        ActionJump = InputActionJumpFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Jump found: %s"), *ActionJump->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Jump"));
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintFinder(TEXT("InputAction'/Game/Inputs/IA_Sprint.IA_Sprint'"));
    if (InputActionSprintFinder.Succeeded())
    {
        ActionSprint = InputActionSprintFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Sprint found: %s"), *ActionSprint->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Sprint"));
    }
}

void AADITLOIS_PlayerController::BeginPlay()
{
    if (UEnhancedInputLocalPlayerSubsystem *inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (inputMappingContext)
        {
            inputSubsystem->AddMappingContext(inputMappingContext, 0);
        }
    }
}

void AADITLOIS_PlayerController::OnPossess(APawn *aPawn)
{
    Super::OnPossess(aPawn);

    playerCharacter = Cast<AADITLOIS_PlayerCharacter>(aPawn);
    Cast<UCharacterMovementComponent>(Cast<AADITLOIS_PlayerCharacter>(aPawn)->GetMovementComponent())->MaxWalkSpeed = 300.0;
    checkf(playerCharacter,
           TEXT("AADITLOIS_PlayerCharacter Cast failed."));

    ServerOnPossess(aPawn);
}

void AADITLOIS_PlayerController::ServerOnPossess_Implementation(APawn *aPawn)
{
    Cast<UCharacterMovementComponent>(Cast<AADITLOIS_PlayerCharacter>(aPawn)->GetMovementComponent())->MaxWalkSpeed = 300.0;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor(64, 192, 64), TEXT("Server is running OnPossess"));
    }
}

void AADITLOIS_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

    if (ActionLook)
    {
        enhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionLook);
    }

    if (ActionMove)
    {
        enhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionMove);
    }

    if (ActionJump)
    {
        enhancedInputComponent->BindAction(ActionJump, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionJump);
    }

    if (ActionSprint)
    {
        enhancedInputComponent->BindAction(ActionSprint, ETriggerEvent::Started, this, &AADITLOIS_PlayerController::OnActionSprintPress);
    }

    if (ActionSprint)
    {
        enhancedInputComponent->BindAction(ActionSprint, ETriggerEvent::Completed, this, &AADITLOIS_PlayerController::OnActionSprintRelease);
    }
}

void AADITLOIS_PlayerController::OnActionLook(const FInputActionValue &Value)
{
    FVector2D look = Value.Get<FVector2D>();
    this->AddYawInput(look.X);
    this->AddPitchInput(look.Y);
    ServerOnActionLook(Value);
}

void AADITLOIS_PlayerController::ServerOnActionLook_Implementation(const FInputActionValue &Value)
{
    // if (GEngine)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor(0, 255, 0), TEXT("Server received RPC to look around"));
    // }

    FVector2D look = Value.Get<FVector2D>();
    this->GetPawn()->AddControllerPitchInput(look.Y);
    this->GetPawn()->AddControllerYawInput(look.X);
}

void AADITLOIS_PlayerController::OnActionMove(const FInputActionValue &Value)
{
    FVector2D move = Value.Get<FVector2D>();
    this->GetPawn()->SetActorRotation(FRotator(this->GetPawn()->GetActorRotation().Pitch,
                                               this->GetPawn()->GetControlRotation().Yaw,
                                               this->GetPawn()->GetActorRotation().Roll));
    this->GetPawn()->AddMovementInput(this->GetPawn()->GetActorForwardVector(), move.Y);
    this->GetPawn()->AddMovementInput(this->GetPawn()->GetActorRightVector(), move.X);
    ServerOnActionMove(Value);
}

void AADITLOIS_PlayerController::ServerOnActionMove_Implementation(const FInputActionValue &Value)
{
    // if (GEngine)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor(0, 255, 0), TEXT("Server received RPC to move"));
    // }

    FVector2D move = Value.Get<FVector2D>();
    this->GetPawn()->SetActorRotation(FRotator(this->GetPawn()->GetActorRotation().Pitch,
                                               this->GetPawn()->GetControlRotation().Yaw,
                                               this->GetPawn()->GetActorRotation().Roll));
    this->GetPawn()->AddMovementInput(this->GetPawn()->GetActorForwardVector(), move.Y);
    this->GetPawn()->AddMovementInput(this->GetPawn()->GetActorRightVector(), move.X);
}

void AADITLOIS_PlayerController::OnActionJump(const FInputActionValue &Value)
{
    if (this->GetPawn())
    {
        Cast<AADITLOIS_PlayerCharacter>(this->GetPawn())->Jump();
    }
    ServerOnActionJump(Value);
}

void AADITLOIS_PlayerController::ServerOnActionJump_Implementation(const FInputActionValue &Value)
{
    if (this->GetPawn())
    {
        Cast<AADITLOIS_PlayerCharacter>(this->GetPawn())->Jump();
    }
}

void AADITLOIS_PlayerController::OnActionSprintPress(const FInputActionValue &Value)
{
    Cast<UCharacterMovementComponent>(Cast<AADITLOIS_PlayerCharacter>(this->GetPawn())->GetMovementComponent())->MaxWalkSpeed = 600.0;
    ServerOnActionSprintPress(Value);
}

void AADITLOIS_PlayerController::ServerOnActionSprintPress_Implementation(const FInputActionValue &Value)
{
    Cast<UCharacterMovementComponent>(Cast<AADITLOIS_PlayerCharacter>(this->GetPawn())->GetMovementComponent())->MaxWalkSpeed = 600.0;
}

void AADITLOIS_PlayerController::OnActionSprintRelease(const FInputActionValue &Value)
{

    Cast<UCharacterMovementComponent>(Cast<AADITLOIS_PlayerCharacter>(this->GetPawn())->GetMovementComponent())->MaxWalkSpeed = 300.0;
    ServerOnActionSprintRelease(Value);
}

void AADITLOIS_PlayerController::ServerOnActionSprintRelease_Implementation(const FInputActionValue &Value)
{
    Cast<UCharacterMovementComponent>(Cast<AADITLOIS_PlayerCharacter>(this->GetPawn())->GetMovementComponent())->MaxWalkSpeed = 300.0;
}
