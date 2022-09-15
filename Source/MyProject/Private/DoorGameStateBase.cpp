// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorGameStateBase.h"

#include "Net/UnrealNetwork.h"

ADoorGameStateBase::ADoorGameStateBase()
{
	bReplicates = true;
	GlobalPlayerName = "foo";
}

void ADoorGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoorGameStateBase, GlobalPlayerName);
}

void ADoorGameStateBase::SetGlobalPlayerName_Server_Implementation(const FString& NewName)
{
	GlobalPlayerName = NewName;
}

void ADoorGameStateBase::SetGlobalPlayerName(const FString& NewGlobalName)
{
		SetGlobalPlayerName_Server(NewGlobalName);
}

void ADoorGameStateBase::GetGlobalPlayerName(FString& Name)
{
	Name = GlobalPlayerName;
}
