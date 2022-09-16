// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgets/MainMenu.h"
#include "Components/WidgetSwitcher.h"

bool UMainMenu::Initialize()
{
	// Initialized assuming that it is going to succeed.
	bool bIsSuccess = true;

	// If parent initialize failed, bIsSuccess should be false
	bIsSuccess &= Super::Initialize();
	if (!bIsSuccess)
	{
		return false;
	}

	StartupMenu->OnMenuButtonClicked.AddDynamic(this, &UMainMenu::UMainMenu::OnStartupMenuButtonClicked_Callback);
	HostGameMenu->OnHostingSettingsAccepted.AddDynamic(this, &UMainMenu::OnHostGameMenuSettingAccepted_Callback);
	HostGameMenu->OnBackButtonClicked.AddDynamic(this, &UMainMenu::BackButtonPressed_Callback);
	
	return true;
}

void UMainMenu::SetSessionController(ISessionController* Controller)
{
	SessionController = Controller;
}

void UMainMenu::OpenStartupMenu()
{
	if(!ensure(MenuSwitcher != nullptr))
		return;
	if(!ensure(StartupMenu != nullptr))
		return;

	UWidget* StartupMenuWidget = Cast<UWidget>(StartupMenu);
	if(ensure(StartupMenuWidget != nullptr))
	{
		MenuSwitcher->SetActiveWidget(StartupMenuWidget);
	}
}

void UMainMenu::OpenHostGameMenu()
{
	if(!ensure(MenuSwitcher != nullptr))
		return;
	if(!ensure(HostGameMenu != nullptr))
		return;

	MenuSwitcher->SetActiveWidget(HostGameMenu);
}

void UMainMenu::InitializeSessionSearch()
{
	if(!ensure(SessionController != nullptr))
		return;

	ShowFindingSessionsWidget(true);

	SessionController->FindGame(3, true);
}

void UMainMenu::QuitGame()
{
	OnQuitButtonPressed.Broadcast();
}

void UMainMenu::ShowFindingSessionsWidget(bool bShow)
{
	ESlateVisibility Visibilty = bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	FindingSession->SetVisibility(Visibilty);
}

void UMainMenu::ShowFindSessionFailed(bool bShow)
{
	ESlateVisibility Visibilty = bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	FindSessionFailed->SetVisibility(Visibilty);
}

void UMainMenu::OnStartupMenuButtonClicked_Callback(EButtonClicked ClickedOn)
{
	switch (ClickedOn)
	{
	case EButtonClicked::None:
		return;

	case EButtonClicked::HostGame:
		OpenHostGameMenu();
		break;

	case EButtonClicked::FindGame:
		InitializeSessionSearch();
		break;

	case EButtonClicked::QuitGame:
		QuitGame();
		break;

	default:
		break;
	}
}

void UMainMenu::OnHostGameMenuSettingAccepted_Callback(FHostGameSessionSettings Settings)
{
	if(!ensure(SessionController != nullptr))
		return;

	OnSessionCreation.Broadcast();
	
	SessionController->HostGame(Settings.MaxNumberOfPlayers, Settings.SessionName, Settings.bIsLAN, Settings.NameOfPlayers);
}

void UMainMenu::BackButtonPressed_Callback()
{
	MenuSwitcher->SetActiveWidget(StartupMenu);
}
