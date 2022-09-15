// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DoorGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayersNameChange, const FString&, PlayersName);

UCLASS(minimalapi)
class ADoorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADoorGameMode();


private:
};



