// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/SessionController.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSessionSubsystem.generated.h"


/**
 * Subsystems Introduced around 4.2x is great help for creation of this kinds of systems. It makes debugging and
 * expanding the systems much easier. 
 */
UCLASS()
class MYPROJECT_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem, public ISessionController
{
	GENERATED_BODY()

public:
	UMultiplayerSessionSubsystem();

	// Interface functions for Widget classes to call from c++
	virtual void HostGame(const int32 MaxNumberOfPlayers, const FName& SessionName, const bool bIsLAN) override;
	virtual void FindGame(int32 MaxSearchResults, bool bIsLanQuery) override;
	virtual void JoinGame() override;

	virtual FOnSessionCreated& GetOnSessionCreated() override;
	virtual FOnSessionDestroyed& GetOnSessionDestroyed() override;
	virtual FOnJoinSession& GetOnJoinSession() override;
	virtual FOnSessionsFound& GetOnSessionFound() override;

	// Controller Functions
	void CreateSession(int32 NumberOfPublicConnections, const FName& SessionName, bool bIsLAN);
	void DestroySession(const FName& SessionName);
	void FindSessions(int32 MaxSearchResults, bool bIsLanQuery);
	void JoinGameSession(const FOnlineSessionSearchResult& SessionToJoin);
	bool TryTravelToCurrentSession(const FName& SessionName);

	// Delegates to notify WidgetManagerSubsystem
	FOnSessionCreated OnSessionCreatedDelegate;
	FOnSessionDestroyed OnSessionDestroyedDelegate;
	FOnSessionsFound OnSessionsFoundDelegate;
	FOnJoinSession OnJoinSessionDelegate;

	
protected:
	// Callbacks for Online Session Interface Delegates
	UFUNCTION()
	void OnSessionCreated_Callback(FName SessionName, bool bIsSuccessful);
	
	UFUNCTION()
	void OnSessionDestroyed_Callback(FName SessionName, bool bIsSuccessful);
	
	UFUNCTION()
	void OnSessionsFound_Callback(bool bIsSuccessful);
	
	void OnJoinSessionComplete_Callback(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	// Online Session Interface Delegates
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	// Handles for Online Session Interface Delegates
	FDelegateHandle CreateSessionCompleteDelegate_Handle;
	FDelegateHandle DestroySessionCompleteDelegate_Handle;
	FDelegateHandle FindSessionsCompleteDelegate_Handle;
	FDelegateHandle JoinSessionCompleteDelegate_Handle;


	// Holds the last state of the SesssionSettings.
	// Using this to keep track of created and updated session settings
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
};
