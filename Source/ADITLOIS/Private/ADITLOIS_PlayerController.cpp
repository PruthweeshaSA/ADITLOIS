// Fill out your copyright notice in the Description page of Project Settings.

#include "ADITLOIS_PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "ADITLOIS_PlayerCharacter.h"
#include "ADITLOIS_PlayerPawn.h"
#include "ADITLOIS_GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "IADITLOIS_Interactable_Interface.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/CharacterMovementComponent.h"

AADITLOIS_PlayerController::AADITLOIS_PlayerController()
{
    bReplicates = true;

    // Use FObjectFinder to find the InputMappingContext
    const TCHAR *locomotionIMCReferencePath = TEXT("InputMappingContext'/Game/Assets/Inputs/IMC_Locomotion.IMC_Locomotion'");
    static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextFinder(locomotionIMCReferencePath);
    if (InputMappingContextFinder.Succeeded())
    {
        inputMappingContext = InputMappingContextFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Mapping Context found: %s"), *inputMappingContext->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Mapping Context"));
    }

    const TCHAR *lookInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_Look.IA_Look'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLookFinder(lookInputActionReferencePath);
    if (InputActionLookFinder.Succeeded())
    {
        ActionLook = InputActionLookFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Look found: %s"), *ActionLook->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Look"));
    }

    const TCHAR *moveInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_Move.IA_Move'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveFinder(moveInputActionReferencePath);
    if (InputActionMoveFinder.Succeeded())
    {
        ActionMove = InputActionMoveFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Move found: %s"), *ActionMove->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Move"));
    }

    const TCHAR *jumpInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_Jump.IA_Jump'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpFinder(jumpInputActionReferencePath);
    if (InputActionJumpFinder.Succeeded())
    {
        ActionJump = InputActionJumpFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Jump found: %s"), *ActionJump->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Jump"));
    }

    const TCHAR *sprintInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_Sprint.IA_Sprint'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSprintFinder(sprintInputActionReferencePath);
    if (InputActionSprintFinder.Succeeded())
    {
        ActionSprint = InputActionSprintFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Sprint found: %s"), *ActionSprint->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Sprint"));
    }

    const TCHAR *interactInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_Interact.IA_Interact'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteractFinder(interactInputActionReferencePath);
    if (InputActionSprintFinder.Succeeded())
    {
        ActionInteract = InputActionInteractFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Interact found: %s"), *ActionInteract->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Interact"));
    }

    const TCHAR *cameraZoomInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_CameraZoom.IA_CameraZoom'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionCameraZoomFinder(cameraZoomInputActionReferencePath);
    if (InputActionCameraZoomFinder.Succeeded())
    {
        ActionCameraZoom = InputActionCameraZoomFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Camera Zoom found: %s"), *ActionCameraZoom->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Camera Zoom"));
    }

    const TCHAR *saveGameInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_SaveGame.IA_SaveGame'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSaveGameFinder(saveGameInputActionReferencePath);
    if (InputActionSaveGameFinder.Succeeded())
    {
        ActionSaveGame = InputActionSaveGameFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Save Game found: %s"), *ActionSaveGame->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Action Save Game"));
    }

    const TCHAR *loadGameInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_LoadGame.IA_LoadGame'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLoadGameFinder(loadGameInputActionReferencePath);
    if (InputActionLoadGameFinder.Succeeded())
    {
        ActionLoadGame = InputActionLoadGameFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Load Game found: %s"), *ActionLoadGame->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Load Game"));
    }

    const TCHAR *switchCharacterInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_SwitchCharacter.IA_SwitchCharacter'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionSwitchCharacterFinder(switchCharacterInputActionReferencePath);
    if (InputActionSwitchCharacterFinder.Succeeded())
    {
        ActionSwitchCharacter = InputActionSwitchCharacterFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Switch character found: %s"), *ActionSwitchCharacter->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Switch Character"));
    }

    const TCHAR *pauseGameInputActionReferencePath = TEXT("InputAction'/Game/Assets/Inputs/IA_PauseGame.IA_PauseGame'");
    static ConstructorHelpers::FObjectFinder<UInputAction> InputActionPauseGameFinder(pauseGameInputActionReferencePath);
    if (InputActionPauseGameFinder.Succeeded())
    {
        ActionPauseGame = InputActionPauseGameFinder.Object;
        UE_LOG(LogTemp, Log, TEXT("Input Action Pause Game found: %s"), *ActionPauseGame->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find Input Pause Game"));
    }
}

void AADITLOIS_PlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (UEnhancedInputLocalPlayerSubsystem *inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (inputMappingContext)
        {
            inputSubsystem->AddMappingContext(inputMappingContext, 0);
        }
    }

    UGameplayStatics::SetGamePaused(GetWorld(), false);

    FInputModeGameOnly InputMode;
    this->SetInputMode(InputMode);
    this->bShowMouseCursor = false;

    this->playerScore = 0;
}

void AADITLOIS_PlayerController::OnPossess(APawn *aPawn)
{
    Super::OnPossess(aPawn);

    TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(aPawn);
    if (!pCharacter)
        return;
    playerPawn = pCharacter;
    if (playerPawn != nullptr)
    {
        Cast<UCharacterMovementComponent>(playerPawn->GetMovementComponent())->MaxWalkSpeed = 300.0;
        ServerOnPossess(aPawn);
    }
    else
    {
        TObjectPtr<AADITLOIS_PlayerPawn> pPawn = Cast<AADITLOIS_PlayerPawn>(aPawn);
        if (playerPawn != nullptr)
        {
            Cast<UFloatingPawnMovement>(playerPawn->GetMovementComponent())->MaxSpeed = 300.0;
            ServerOnPossess(aPawn);
        }
    }
    // checkf(playerCharacter,
    //        TEXT("AADITLOIS_PlayerCharacter Cast failed."));
}

void AADITLOIS_PlayerController::ServerOnPossess_Implementation(APawn *aPawn)
{
    TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(aPawn);
    if (!pCharacter)
        return;
    if (pCharacter != nullptr)
    {
        Cast<UCharacterMovementComponent>(pCharacter->GetMovementComponent())->MaxWalkSpeed = 300.0;
    }
    else
    {
        TObjectPtr<AADITLOIS_PlayerPawn> pPawn = Cast<AADITLOIS_PlayerPawn>(aPawn);
        if (!pPawn)
            return;
        if (pPawn != nullptr)
        {
            Cast<UFloatingPawnMovement>(pPawn->GetMovementComponent())->MaxSpeed = 300.0;
        }
    }

    Cast<AADITLOIS_GameModeBase>(GetWorld()->GetAuthGameMode())->SpawnBots();
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    UE_LOG(LogTemp, Log, TEXT("Server is running OnPossess"));
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

    if (ActionInteract)
    {
        enhancedInputComponent->BindAction(ActionInteract, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionInteract);
    }

    if (ActionCameraZoom)
    {
        enhancedInputComponent->BindAction(ActionCameraZoom, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionCameraZoom);
    }

    if (ActionSaveGame)
    {
        enhancedInputComponent->BindAction(ActionSaveGame, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionSaveGame);
    }

    if (ActionLoadGame)
    {
        enhancedInputComponent->BindAction(ActionLoadGame, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionLoadGame);
    }

    if (ActionSwitchCharacter)
    {
        enhancedInputComponent->BindAction(ActionSwitchCharacter, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionSwitchCharacter);
    }

    if (ActionPauseGame)
    {
        enhancedInputComponent->BindAction(ActionPauseGame, ETriggerEvent::Triggered, this, &AADITLOIS_PlayerController::OnActionPauseGame);
    }
}

void AADITLOIS_PlayerController::OnActionLook(const FInputActionValue &Value)
{
    FRotator actorRotation = this->GetPawn()->GetActorRotation();

    FVector2D look = Value.Get<FVector2D>();
    this->GetPawn()->AddControllerYawInput(look.X);
    this->GetPawn()->AddControllerPitchInput(look.Y);
    ServerOnActionLook(this->GetControlRotation(), this->GetPawn()->GetActorRotation());
}

void AADITLOIS_PlayerController::ServerOnActionLook_Implementation(FRotator controllerRotation, FRotator characterRotation)
{
    this->playerControllerRotation = controllerRotation;
    this->GetPawn()->SetActorRotation(characterRotation);
}

void AADITLOIS_PlayerController::OnActionMove(const FInputActionValue &Value)
{

    FVector2D move = Value.Get<FVector2D>();
    this->GetPawn()->SetActorRotation(FRotator(this->GetPawn()->GetActorRotation().Pitch,
                                               this->GetPawn()->GetControlRotation().Yaw,
                                               this->GetPawn()->GetActorRotation().Roll));
    this->GetPawn()->AddMovementInput(this->GetPawn()->GetActorForwardVector(), move.Y);
    this->GetPawn()->AddMovementInput(this->GetPawn()->GetActorRightVector(), move.X);

    ServerOnActionMove(this->GetPawn()->GetActorLocation(), this->GetPawn()->GetActorRotation());
}

void AADITLOIS_PlayerController::ServerOnActionMove_Implementation(FVector characterLocation, FRotator characterRotation)
{
    this->GetPawn()->SetActorLocation(characterLocation);
    this->GetPawn()->SetActorRotation(characterRotation);
}

void AADITLOIS_PlayerController::OnActionJump(const FInputActionValue &Value)
{

    if (this->GetPawn())
    {
        TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
        if (!pCharacter)
            return;
        pCharacter->Jump();
    }
}

void AADITLOIS_PlayerController::OnActionSprintPress(const FInputActionValue &Value)
{
    if (true || (this->HasAuthority()))
    {
        TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
        if (!pCharacter)
        {
            TObjectPtr<APawn> pPawn = Cast<APawn>(this->GetPawn());
            Cast<UFloatingPawnMovement>(pPawn->GetMovementComponent())->MaxSpeed = 1200.0;
            return;
        }
        Cast<UCharacterMovementComponent>(pCharacter->GetMovementComponent())->MaxWalkSpeed = 600.0;
    }
    if (!(this->HasAuthority()))
    {
        ServerOnActionSprintPress(Value);
    }
}

void AADITLOIS_PlayerController::ServerOnActionSprintPress_Implementation(const FInputActionValue &Value)
{
    TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
    if (!pCharacter)
    {
        TObjectPtr<APawn> pPawn = Cast<APawn>(this->GetPawn());
        Cast<UFloatingPawnMovement>(pPawn->GetMovementComponent())->MaxSpeed = 1200.0;
        return;
    }
    Cast<UCharacterMovementComponent>(pCharacter->GetMovementComponent())->MaxWalkSpeed = 600.0;
}

void AADITLOIS_PlayerController::OnActionSprintRelease(const FInputActionValue &Value)
{
    if (true || (this->HasAuthority()))
    {
        TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
        if (!pCharacter)
        {
            TObjectPtr<APawn> pPawn = Cast<APawn>(this->GetPawn());
            Cast<UFloatingPawnMovement>(pPawn->GetMovementComponent())->MaxSpeed = 300.0;
            return;
        }
        Cast<UCharacterMovementComponent>(pCharacter->GetMovementComponent())->MaxWalkSpeed = 300.0;
    }
    if (!(this->HasAuthority()))
    {
        ServerOnActionSprintRelease(Value);
    }
}

void AADITLOIS_PlayerController::ServerOnActionSprintRelease_Implementation(const FInputActionValue &Value)
{
    TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
    if (!pCharacter)
    {
        TObjectPtr<APawn> pPawn = Cast<APawn>(this->GetPawn());
        Cast<UFloatingPawnMovement>(pPawn->GetMovementComponent())->MaxSpeed = 300.0;
        return;
    }
    Cast<UCharacterMovementComponent>(pCharacter->GetMovementComponent())->MaxWalkSpeed = 300.0;
}

void AADITLOIS_PlayerController::OnActionInteract(const FInputActionValue &Value)
{
    if (true || (this->HasAuthority()))
    {
        if (this->GetPawn())
        {
            TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
            if (!pCharacter)
                return;
            TObjectPtr<AActor> actorToInteractWith = pCharacter->interactionTarget;
            if (actorToInteractWith && actorToInteractWith->GetIsReplicated())
            {
                if (actorToInteractWith->GetClass()->ImplementsInterface(UADITLOIS_Interactable_Interface::StaticClass()))
                {
                    // actorToInteractWith->Destroy();
                    // this->playerScore += 1;

                    IADITLOIS_Interactable_Interface::Execute_Interact(actorToInteractWith, this->GetPawn());
                }
            }
            else
            {
                if (actorToInteractWith == nullptr)
                {
                    UE_LOG(LogTemp, Warning, TEXT("No actor to interact with."));
                }
                else if (!actorToInteractWith->GetIsReplicated())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Actor to interact with is not replicated."));
                }
            }
        }
    }
    if (!(this->HasAuthority()))
    {
        ServerOnActionInteract(Value);
    }
}

void AADITLOIS_PlayerController::ServerOnActionInteract_Implementation(const FInputActionValue &Value)
{
    if (this->GetPawn())
    {
        TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
        if (!pCharacter)
            return;
        TObjectPtr<AActor> actorToInteractWith = pCharacter->interactionTarget;
        if (actorToInteractWith && actorToInteractWith->GetIsReplicated())
        {
            if (actorToInteractWith->GetClass()->ImplementsInterface(UADITLOIS_Interactable_Interface::StaticClass()))
            {
                // actorToInteractWith->Destroy();
                // this->playerScore += 1;

                IADITLOIS_Interactable_Interface::Execute_Interact(actorToInteractWith, this->GetPawn());
            }
        }
    }
}

void AADITLOIS_PlayerController::OnActionCameraZoom(const FInputActionValue &Value)
{
    if (true || this->HasAuthority())
    {
        if (this->GetPawn() == nullptr)
        {
            return;
        }
        TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
        if (!pCharacter)
            return;
        float move = Value.Get<float>();
        TObjectPtr<USpringArmComponent> characterSpringArm = pCharacter->springArm;
        if (move > 0.0f)
        {
            characterSpringArm->TargetArmLength = characterSpringArm->TargetArmLength >= 150.0f ? characterSpringArm->TargetArmLength - 30.0f : 0.0f;
            if (characterSpringArm->TargetArmLength == 0.0f)
            {
                pCharacter->bUseControllerRotationYaw = true;
                characterSpringArm->AttachToComponent(pCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("EyeSocket")));
            }
        }
        else if (move < 0.0f)
        {
            characterSpringArm->TargetArmLength = characterSpringArm->TargetArmLength < 420.0f ? characterSpringArm->TargetArmLength + 30.0f : 450.0f;
            if (characterSpringArm->TargetArmLength < 120.0f)
            {
                characterSpringArm->TargetArmLength = 120.0f;
            }
            pCharacter->bUseControllerRotationYaw = false;
            characterSpringArm->AttachToComponent(pCharacter->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("EyeSocket")));
        }
        float yOffset = characterSpringArm->TargetArmLength >= 120.0f ? 10.0f + (characterSpringArm->TargetArmLength / 6.0f) : 0.0f;
        float zOffset = characterSpringArm->TargetArmLength >= 120.0f ? 70.0f : 0.0f;
        characterSpringArm->SocketOffset = FVector(0.0f, yOffset, zOffset);
    }
    if (!(this->HasAuthority()))
    {
        ServerOnActionCameraZoom(Value);
    }
}

void AADITLOIS_PlayerController::ServerOnActionCameraZoom_Implementation(const FInputActionValue &Value)
{
    if (this->GetPawn() == nullptr)
    {
        return;
    }
    TObjectPtr<AADITLOIS_PlayerCharacter> pCharacter = Cast<AADITLOIS_PlayerCharacter>(this->GetPawn());
    if (!pCharacter)
        return;
    float move = Value.Get<float>();
    TObjectPtr<USpringArmComponent> characterSpringArm = pCharacter->springArm;
    if (move > 0.0f)
    {
        characterSpringArm->TargetArmLength = characterSpringArm->TargetArmLength >= 150.0f ? characterSpringArm->TargetArmLength - 30.0f : 0.0f;
        if (characterSpringArm->TargetArmLength == 0.0f)
        {
            pCharacter->bUseControllerRotationYaw = true;
            characterSpringArm->AttachToComponent(pCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("EyeSocket")));
        }
    }
    else if (move < 0.0f)
    {
        characterSpringArm->TargetArmLength = characterSpringArm->TargetArmLength < 420.0f ? characterSpringArm->TargetArmLength + 30.0f : 450.0f;
        if (characterSpringArm->TargetArmLength < 120.0f)
        {
            characterSpringArm->TargetArmLength = 120.0f;
        }
        pCharacter->bUseControllerRotationYaw = false;
        characterSpringArm->AttachToComponent(pCharacter->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("EyeSocket")));
    }
    float yOffset = characterSpringArm->TargetArmLength >= 120.0f ? 10.0f + (characterSpringArm->TargetArmLength / 6.0f) : 0.0f;
    float zOffset = characterSpringArm->TargetArmLength >= 120.0f ? 70.0f : 0.0f;
    characterSpringArm->SocketOffset = FVector(0.0f, yOffset, zOffset);

    ForceNetUpdate();
}

void AADITLOIS_PlayerController::OnActionSaveGame()
{
    if (this->HasAuthority())
    {
        Cast<AADITLOIS_GameModeBase>(this->GetWorld()->GetAuthGameMode())->SaveGame(this);
    }
    else
    {
        ServerOnActionSaveGame();
    }
}

void AADITLOIS_PlayerController::ServerOnActionSaveGame_Implementation()
{
    Cast<AADITLOIS_GameModeBase>(this->GetWorld()->GetAuthGameMode())->SaveGame(this);
}

void AADITLOIS_PlayerController::OnActionLoadGame()
{
    if (this->HasAuthority())
    {
        Cast<AADITLOIS_GameModeBase>(this->GetWorld()->GetAuthGameMode())->LoadGame(this);
    }
    else
    {
        ServerOnActionLoadGame();
    }
    this->SetControlRotation(this->playerControllerRotation);
}

void AADITLOIS_PlayerController::ServerOnActionLoadGame_Implementation()
{
    Cast<AADITLOIS_GameModeBase>(this->GetWorld()->GetAuthGameMode())->LoadGame(this);
}

void AADITLOIS_PlayerController::OnActionSwitchCharacter()
{
    ServerOnActionSwitchCharacter();
}

void AADITLOIS_PlayerController::ServerOnActionSwitchCharacter_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Called Server Switch Character."));

    FString LionessPath = "/Game/Blueprints/Character_Blueprints/BPI_ADITLOIS_PlayerPawn_Lioness.BPI_ADITLOIS_PlayerPawn_Lioness_C";
    // FString LionessPath = "/Game/Blueprints/Character_Blueprints/BP_ADITLOIS_PlayerCharacter_Lioness.BP_ADITLOIS_PlayerCharacter_Lioness_C";
    // FString LionessPath = "/Game/Blueprints/Character_Blueprints/BP_ADITLOIS_QuadrupedPawn.BP_ADITLOIS_QuadrupedPawn_C";

    FString ErectusPath = "/Game/Blueprints/Character_Blueprints/BP_ADITLOIS_PlayerCharacter.BP_ADITLOIS_PlayerCharacter_C";

    APawn *ExistingPawn = this->GetPawn();
    if (!ExistingPawn)
        return;

    UClass *NewPawnClass = LoadClass<APawn>(nullptr, *LionessPath) == (ExistingPawn->GetClass()) ? LoadClass<APawn>(nullptr, *ErectusPath) : LoadClass<APawn>(nullptr, *LionessPath);
    if (!NewPawnClass)
        return;

    UWorld *CurrentWorld = this->GetWorld();
    if (!CurrentWorld)
        return;

    // Unpossess before destroying

    FVector NewSpawnLocation = ExistingPawn ? ExistingPawn->GetActorLocation() : FVector::ZeroVector;
    FRotator NewSpawnRotation = ExistingPawn ? ExistingPawn->GetActorRotation() : FRotator::ZeroRotator;
    this->UnPossess();
    ExistingPawn->Destroy(); // Safely destroy after

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = ExistingPawn;

    APawn *NewPawn = CurrentWorld->SpawnActor<APawn>(NewPawnClass, NewSpawnLocation, NewSpawnRotation, SpawnParams);
    if (!NewPawn)
        return;

    this->Possess(NewPawn); // Replicated automatically to all clients

    UE_LOG(LogTemp, Warning, TEXT("Called Server Switch Character and successfully switched to spawned pawn."));
}

void AADITLOIS_PlayerController::OnActionPauseGame()
{
    UClass *LoadedWidgetClass = LoadClass<UUserWidget>(
        nullptr,
        TEXT("/Game/Blueprints/Widget_Blueprints/BP_PauseMenu_Widget.BP_PauseMenu_Widget_C") // Note the _C at the end!
    );
    if (LoadedWidgetClass)
    {
        UUserWidget *WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), LoadedWidgetClass);
        if (WidgetInstance != nullptr)
        {
            WidgetInstance->AddToViewport();

            // Set input mode to UI only or game and UI
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(WidgetInstance->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            this->SetInputMode(InputMode);

            // Show the mouse cursor
            this->bShowMouseCursor = true;
        }
    }
}

void AADITLOIS_PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AADITLOIS_PlayerController, playerPawn);
    DOREPLIFETIME(AADITLOIS_PlayerController, playerScore);
    DOREPLIFETIME(AADITLOIS_PlayerController, playerControllerRotation);
}