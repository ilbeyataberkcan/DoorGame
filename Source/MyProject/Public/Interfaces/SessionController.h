// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/Interface.h"
#include "SessionController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionCreated, bool, bSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDestroyed, bool, bSuccessful);
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

	virtual FOnSessionCreated& GetOnSessionCreated() = 0;
	virtual FOnSessionsFound& GetOnSessionFound() = 0;
	virtual FOnSessionDestroyed& GetOnSessionDestroyed() = 0;
	virtual FOnJoinSession& GetOnJoinSession() = 0;
	
	virtual void HostGame(const int32 MaxNumberOfPlayers, const FName& SessionName, const bool bIsLAN) = 0;
	virtual void FindGame(int32 MaxSearchResults, bool bIsLanQuery) = 0;
	virtual void JoinGame() = 0;
};
