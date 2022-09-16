// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/Interface.h"
#include "SessionController.generated.h"

/*
 *	Make sure every session controller broadcasts it states.
 *	We could directly bind events to OnlineSessionInterface's delegates. However this way we get the chance to customize
 *	those delegates' signatures. And event make them capable of notifying blueprints
 *
 *	They are identical to OnlineSesionInterface delegates. Also not capable of broadcasting to blueprint because all the
 *	all the UI actions handled in C++ and FOnlineSessionSearchResult and EOnJoinSessionComplete are not UCLASSes. So,
 *	exposing them to blueprints require wrapper classes/structs and some functions to retrieve the information.
 *
 *	Create the Delegate signature variables in the children of this interface and return them in getter functions
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionDestroyed, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSessionsFound, const TArrayView<FOnlineSessionSearchResult>&, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnJoinSession, FName, EOnJoinSessionCompleteResult::Type);


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USessionController : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API ISessionController
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Get the delegates from SessionController (MultiplayerSessionSubsystem at the moment)
	virtual FOnSessionCreated& GetOnSessionCreated() = 0;
	virtual FOnSessionsFound& GetOnSessionFound() = 0;
	virtual FOnSessionDestroyed& GetOnSessionDestroyed() = 0;
	virtual FOnJoinSession& GetOnJoinSession() = 0;
	
	virtual void HostGame(const int32 MaxNumberOfPlayers, const FName& SessionName, const bool bIsLAN, const FString& NameOfPlayers) = 0;
	virtual void FindGame(int32 MaxSearchResults, bool bIsLanQuery) = 0;
	virtual void JoinGame(const FOnlineSessionSearchResult& Session) = 0;
};
