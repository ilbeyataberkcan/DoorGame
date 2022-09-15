// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineFramework/MultiplayerSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"

// Binding our callback functions to Online Session Interface's delegate.
UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
	: CreateSessionCompleteDelegate( FOnCreateSessionCompleteDelegate::CreateUObject( this, &UMultiplayerSessionSubsystem::OnSessionCreated_Callback))
	, DestroySessionCompleteDelegate( FOnDestroySessionCompleteDelegate::CreateUObject( this, &UMultiplayerSessionSubsystem::OnSessionDestroyed_Callback))
	, FindSessionsCompleteDelegate( FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnSessionsFound_Callback))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionSubsystem::OnJoinSessionComplete_Callback))
{
}

void UMultiplayerSessionSubsystem::HostGame(const int32 MaxNumberOfPlayers, const FName& SessionName, const bool bIsLAN)
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if(!SessionInterface.IsValid())
	{
		OnSessionCreatedDelegate.Broadcast(false);
		return;
	}

	const FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SessionName);
	if(ExistingSession == nullptr)
	{
		CreateSession(MaxNumberOfPlayers, SessionName, bIsLAN);
	}
	else
	{
		DestroySession(SessionName);
	}
}

void UMultiplayerSessionSubsystem::FindGame(int32 MaxSearchResults, bool bIsLanQuery)
{
	FindSessions(MaxSearchResults, bIsLanQuery);
}

void UMultiplayerSessionSubsystem::JoinGame()
{
	
}

FOnSessionCreated& UMultiplayerSessionSubsystem::GetOnSessionCreated()
{
	return OnSessionCreatedDelegate;
}

FOnSessionDestroyed& UMultiplayerSessionSubsystem::GetOnSessionDestroyed()
{
	return OnSessionDestroyedDelegate;
}

FOnJoinSession& UMultiplayerSessionSubsystem::GetOnJoinSession()
{
	return OnJoinSessionDelegate;
}

FOnSessionsFound& UMultiplayerSessionSubsystem::GetOnSessionFound()
{
	return OnSessionsFoundDelegate;
}


//////////////////// BLUEPRINT EXPOSED FUNCTIONS (Controllers) ////////////////////////////

void UMultiplayerSessionSubsystem::CreateSession(int32 NumberOfPublicConnections, const FName& SessionName, bool bIsLAN)
{
	// There are two versions of getting Session Interface. One declared in Online.h the other in OnlineSubsystemUtils.h
	// We want to use the OnlineSubsystemUtils.h version because it takes the Editor sessions into account. Because
	// Editor can have multiple UWorld instances at the same time. 
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		// Session Create Failed. Couldn't get session Interface.
		OnSessionCreatedDelegate.Broadcast(false);
		return;
	}

	// Initialize Session Settings with session creation.
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	LastSessionSettings->NumPublicConnections = NumberOfPublicConnections; // Number of Players Allowed
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bIsLANMatch = bIsLAN; // Is this match available only to local network
	LastSessionSettings->bShouldAdvertise = true; // Whether or not this match can be found in the network


	// Add our CreateSessionCompleteDelegate to delegate list of SessionInterface.
	// Then get the handle so we can remove it from the list later on.
	CreateSessionCompleteDelegate_Handle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		CreateSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession,
	                                     *LastSessionSettings))
	{
		// Session Creation failed

		// We failed to create session and we don't need to listen for creation complete events anymore.
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate_Handle);
		OnSessionCreatedDelegate.Broadcast(false);
	}
}

//
void UMultiplayerSessionSubsystem::DestroySession(const FName& SessionName)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		// Session Destroy failed. Couldn't get session Interface
		OnSessionDestroyedDelegate.Broadcast(false);
		return;
	}

	DestroySessionCompleteDelegate_Handle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
		DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(SessionName))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate_Handle);

		OnSessionDestroyedDelegate.Broadcast(false);
	}
}

//
void UMultiplayerSessionSubsystem::FindSessions(int32 MaxSearchResults, bool bIsLanQuery)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnSessionsFoundDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionsCompleteDelegate_Handle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = bIsLanQuery;

	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate_Handle);
		OnSessionsFoundDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

//
void UMultiplayerSessionSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionToJoin)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		OnJoinSessionDelegate.Broadcast(FName() ,EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteDelegate_Handle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession,
	                                  SessionToJoin))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate_Handle);
		OnJoinSessionDelegate.Broadcast(FName() ,EOnJoinSessionCompleteResult::UnknownError);
	}
}

//////////////////// ONLINE SESSION INTERFACE CALLBACK FUNCTIONS ////////////////////////////


void UMultiplayerSessionSubsystem::OnSessionsFound_Callback(bool bIsSuccessful)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());

	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate_Handle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0 || !bIsSuccessful)
	{
		OnSessionsFoundDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	JoinGameSession(LastSessionSearch->SearchResults[0]);
	
	OnSessionsFoundDelegate.Broadcast(LastSessionSearch->SearchResults, bIsSuccessful);
}

void UMultiplayerSessionSubsystem::OnJoinSessionComplete_Callback(FName SessionName,
	EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate_Handle);
	}

	TryTravelToCurrentSession(SessionName);

	OnJoinSessionDelegate.Broadcast(SessionName, Result);
}

//
void UMultiplayerSessionSubsystem::OnSessionDestroyed_Callback(FName SessionName, bool bIsSuccessful)
{
	
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());

	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate_Handle);
	}

	if(bIsSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Destroyed : %s"), *SessionName.ToString())
		CreateSession(LastSessionSettings->NumPublicConnections, SessionName, LastSessionSettings->bIsLANMatch);
	}
	
	OnSessionDestroyedDelegate.Broadcast(bIsSuccessful);
}

//
void UMultiplayerSessionSubsystem::OnSessionCreated_Callback(FName SessionName, bool bIsSuccessful)
{
	if (!bIsSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Failed: %s"), *SessionName.ToString())
		return;
	}
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());

	if (SessionInterface)
	{
		// Session is created and we get the notify for it. So, we can remove our delegate from the SessionInterface's
		// delegate list
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate_Handle);
	}

	GetWorld()->ServerTravel("/Game/Levels/LobbyLevel?listen", true);

	OnSessionCreatedDelegate.Broadcast(bIsSuccessful);
}

//
bool UMultiplayerSessionSubsystem::TryTravelToCurrentSession(const FName& SessionName)
{
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface.IsValid())
	{
		return false;
	}

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		return false;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
	return true;
}