// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerNameState.h"

void APlayerNameState::SetGlobalPlayerName_Implementation(const FString& NewName)
{
	SetPlayerName(NewName);
}
