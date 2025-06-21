#include "ADITLOIS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "AdvancedSessionsLibrary.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"

void UADITLOIS_GameInstance::Init()
{
    Super::Init();
}

void UADITLOIS_GameInstance::HostGameSession(FName SessionName, int32 MaxPlayers)
{
    TArray<FSessionPropertyKeyPair> ExtraSettings;

    // Add custom session setting: SESSION_NAME
    FSessionPropertyKeyPair SessionNameSetting;
    SessionNameSetting.Key = FName("ServerName");
    SessionNameSetting.Data.SetValue(SessionName.ToString());
    ExtraSettings.Add(SessionNameSetting);

    APlayerController *PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Failed to get PlayerController!"));
        }
        return;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Got PlayerController."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Got PlayerController!"));
        }
    }

    StoredProxy = UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(
        this,
        ExtraSettings,
        PC,
        MaxPlayers,
        0,     // PrivateConnections
        false, // bUseLAN
        true,  // bAllowInvites
        false, // bIsDedicated
        true,  // bUsePresence
        true,  // bUseLobbiesIfAvailable
        true,  // bAllowJoinViaPresence
        false, // bAllowJoinViaPresenceFriendsOnly
        false, // bAntiCheatProtected
        false, // bUsesStats
        true,  // bShouldAdvertise
        false, // bUseLobbiesVoiceChatIfAvailable
        false  // Set bStartAfterCreate = false
    );

    if (!StoredProxy)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateAdvancedSession returned nullptr!"));
    }
    if (StoredProxy)
    {
        StoredProxy->OnSuccess.AddDynamic(this, &UADITLOIS_GameInstance::OnSessionCreatedSuccess);
        StoredProxy->OnFailure.AddDynamic(this, &UADITLOIS_GameInstance::OnSessionCreatedFailure);

        UE_LOG(LogTemp, Log, TEXT("Routed OnSuccess and OnFailure to functions."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Routed OnSuccess and OnFailure to functions!"));
        }

        UE_LOG(LogTemp, Warning, TEXT("StoredProxy is valid and bound, now activating session creation..."));

        // ✅ ACTIVATE the proxy manually
        StoredProxy->Activate();
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Session creation proxy was null!"));
        }

        UE_LOG(LogTemp, Error, TEXT("Session creation proxy was null."));
    }
}

void UADITLOIS_GameInstance::OnSessionCreatedSuccess()
{
    UE_LOG(LogTemp, Log, TEXT("Session created successfully"));
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Session created successfully!"));
    }

    // Correct string: just path, no class prefix or suffix
    FSoftObjectPath LevelPath(TEXT("/Game/Assets/Levels/UnrealsIdeaofAnEmptyLevel"));

    // Load synchronously
    UObject *LoadedObject = LevelPath.TryLoad();

    if (LoadedObject)
    {
        FString MapPath = LevelPath.GetLongPackageName(); // Returns "/Game/Assets/Levels/UnrealsIdeaofAnEmptyLevel"
        UE_LOG(LogTemp, Warning, TEXT("Level loaded. Opening: %s"), *MapPath);

        UGameplayStatics::OpenLevel(GetWorld(), FName(*MapPath), true, FString(TEXT("listen")));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Level asset failed to load from path: %s"), *LevelPath.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Level asset failed to load!"));
        }
    }

    // Prevents memory leak
    if (StoredProxy)
    {
        StoredProxy->RemoveFromRoot();
        StoredProxy = nullptr;
    }
}

void UADITLOIS_GameInstance::OnSessionCreatedFailure()
{
    UE_LOG(LogTemp, Error, TEXT("Failed to create session"));
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor(64, 255, 64), TEXT("Failed to create session!"));
    }

    // Prevents memory leak
    if (StoredProxy)
    {
        StoredProxy->RemoveFromRoot();
        StoredProxy = nullptr;
    }
}

void UADITLOIS_GameInstance::JoinGameSession(const FBlueprintSessionResult &SessionResult)
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
        return;

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
        return;

    APlayerController *PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
        return;

    JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UADITLOIS_GameInstance::OnJoinSessionComplete));

    SessionInterface->JoinSession(*PC->GetLocalPlayer()->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult.OnlineResult);
}

void UADITLOIS_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
        return;

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
        return;

    // Clean up the delegate
    SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);

    FString ConnectString;
    if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
    {
        APlayerController *PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->ClientTravel(ConnectString, TRAVEL_Absolute);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Could not get resolved connect string."));
    }
}

