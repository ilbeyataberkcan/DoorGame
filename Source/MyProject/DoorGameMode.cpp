// Copyright Epic Games, Inc. All Rights Reserved.

#include "DoorGameMode.h"
#include "DoorGameCharacter.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

ADoorGameMode::ADoorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

}

