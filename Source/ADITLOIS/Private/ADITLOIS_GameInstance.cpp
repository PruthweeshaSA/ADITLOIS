#include "ADITLOIS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "AdvancedSessionsLibrary.h"
#include "CreateSessionCallbackProxyAdvanced.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"
#include "FindSessionsCallbackProxy.h"
#include "Engine/LocalPlayer.h"

void UADITLOIS_GameInstance::Init()
{
    Super::Init();
}

void UADITLOIS_GameInstance::Shutdown()
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // Destroy the session so we can rejoin in the next PIE run
            SessionInterface->DestroySession(NAME_GameSession);
        }
    }

    Super::Shutdown();
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
        return;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Got PlayerController."));
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
        true   // Set bStartAfterCreate = false
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
        UE_LOG(LogTemp, Warning, TEXT("StoredProxy is valid and bound, now activating session creation..."));

        // ✅ ACTIVATE the proxy manually
        StoredProxy->Activate();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Session creation proxy was null."));
    }
}

void UADITLOIS_GameInstance::OnSessionCreatedSuccess()
{
    UE_LOG(LogTemp, Log, TEXT("Session created successfully"));

    // Correct string: just path, no class prefix or suffix
    FSoftObjectPath LevelPath(TEXT("/Game/Assets/Levels/FirstLevel"));

    // Load synchronously
    UObject *LoadedObject = LevelPath.TryLoad();

    if (LoadedObject)
    {
        FString MapPath = LevelPath.GetLongPackageName(); // Returns "/Game/Assets/Levels/FirstLevel"
        UE_LOG(LogTemp, Warning, TEXT("Level loaded. Opening: %s"), *MapPath);

        UGameplayStatics::OpenLevel(GetWorld(), FName(*MapPath), true, FString(TEXT("listen")));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Level asset failed to load from path: %s"), *LevelPath.ToString());
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
    // Prevents memory leak
    if (StoredProxy)
    {
        StoredProxy->RemoveFromRoot();
        StoredProxy = nullptr;
    }
}

// The implementation of the function called from your widget
// ADITLOIS_GameInstance.cpp (Final, Corrected Implementation)

void UADITLOIS_GameInstance::JoinGameSession(const FBlueprintSessionResult &SessionResult)
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("Online Subsystem is not available. Cannot join session."));
        return;
    }

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Session Interface is invalid. Cannot join session."));
        return;
    }

    // 1. Get the ULocalPlayer object associated with the GameInstance's world.
    ULocalPlayer *LocalPlayer = GetFirstGamePlayer();
    if (!LocalPlayer)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not get a valid Local Player. Cannot join session."));
        OnJoinSessionCompleted(NAME_None, EOnJoinSessionCompleteResult::UnknownError);
        return;
    }

    // 2. Bind the delegate to our callback function
    JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UADITLOIS_GameInstance::OnJoinSessionCompleted));

    // 3. CORRECTED: Access the native result using the confirmed member name 'OnlineResult'
    const FOnlineSessionSearchResult *SearchResult = &SessionResult.OnlineResult;

    if (!SearchResult || !SearchResult->IsValid()) // Added check for validity
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid session search result provided via OnlineResult member."));
        OnJoinSessionCompleted(NAME_None, EOnJoinSessionCompleteResult::UnknownError);
        return;
    }

    // 4. Start the latent operation
    bool bSuccess = SessionInterface->JoinSession(0, NAME_GameSession, *SearchResult);

    if (!bSuccess)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to start the native JoinSession call immediately."));
        OnJoinSessionCompleted(NAME_None, EOnJoinSessionCompleteResult::UnknownError);
    }
}

// The callback executed when the latent operation finishes
void UADITLOIS_GameInstance::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // Clear the delegate binding immediately after it fires
            SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
        }
    }

    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully joined session: %s"), *SessionName.ToString());
        TravelToSession(SessionName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to join session. Result: %d"), (int32)Result);
        // Handle failure, e.g., show a UI message
    }
}

// Helper function to connect to the session's address
void UADITLOIS_GameInstance::TravelToSession(FName SessionName)
{
    IOnlineSubsystem *Subsystem = IOnlineSubsystem::Get();
    if (!Subsystem)
        return;

    IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
    if (!SessionInterface.IsValid())
        return;

    FString ConnectInfo;

    // 1. Get the connection string/URL
    if (SessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
    {
        UE_LOG(LogTemp, Log, TEXT("Traveling to URL: %s"), *ConnectInfo);

        // 2. Perform the Client Travel
        if (APlayerController *PlayerController = GetWorld()->GetFirstPlayerController())
        {
            // ClientTravel uses the URL we just retrieved
            PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get connection info for session: %s"), *SessionName.ToString());
    }
}
