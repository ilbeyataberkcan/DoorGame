// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgets/HostGameMenu.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/SpinBox.h"

bool UHostGameMenu::Initialize()
{
	bool bIsSuccess = true;
	bIsSuccess &= Super::Initialize();
	if(!bIsSuccess)
	{
		return false;
	}

	CreateGameButton->OnClicked.AddDynamic(this, &UHostGameMenu::OnCreateGameButtonClicked_Callback);
	BackButton->OnClicked.AddDynamic(this, &UHostGameMenu::OnBackButtonClicked_Callback);
	
	return true;
}

void UHostGameMenu::OnCreateGameButtonClicked_Callback()
{

	FHostGameSessionSettings Settings;
	
	Settings.SessionName = FName(ServerName_Input->GetText().ToString());
	Settings.MaxNumberOfPlayers = MaxNumberOfPlayers->GetValue();
	Settings.bIsLAN = bIsLAN->IsChecked();
	
	OnHostingSettingsAccepted.Broadcast(Settings);
}

void UHostGameMenu::OnBackButtonClicked_Callback()
{
	// Reset input fields to defaults 
	ServerName_Input->SetText(FText::FromString(""));
	MaxNumberOfPlayers->SetValue(2);
	bIsLAN->SetIsChecked(true);
	
	OnBackButtonClicked.Broadcast();
}
