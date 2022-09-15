// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgets/StartupMenu.h"

#include "Components/Button.h"

bool UStartupMenu::Initialize()
{
	bool bIsSuccess = true;
	bIsSuccess &= Super::Initialize();
	if(!bIsSuccess)
		return false;

	HostGame->OnClicked.AddDynamic(this, &UStartupMenu::OnHostGameButtonClicked_Callback);
	FindGame->OnClicked.AddDynamic(this, &UStartupMenu::OnFindGameButtonClicked_Callback);
	QuitGame->OnClicked.AddDynamic(this, &UStartupMenu::OnQuitGameButtonClicked_Callback);

	return true;
}

void UStartupMenu::OnHostGameButtonClicked_Callback()
{
	OnMenuButtonClicked.Broadcast(EButtonClicked::HostGame);
}

void UStartupMenu::OnFindGameButtonClicked_Callback()
{
	OnMenuButtonClicked.Broadcast(EButtonClicked::FindGame);
}

void UStartupMenu::OnQuitGameButtonClicked_Callback()
{
	OnMenuButtonClicked.Broadcast(EButtonClicked::QuitGame);
}
