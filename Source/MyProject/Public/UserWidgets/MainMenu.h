// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HostGameMenu.h"
#include "StartupMenu.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/SessionController.h"
#include "MainMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSessionCreation);

/**
 * 
 */
UCLASS()
class MYPROJECT_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSessionController(ISessionController* Controller);
	
	void OpenStartupMenu();
	void OpenHostGameMenu();
	void InitializeSessionSearch();
	void QuitGame();

	FOnSessionCreation OnSessionCreation;

protected:
	virtual bool Initialize();

private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher*	MenuSwitcher;
	
	UPROPERTY(meta=(BindWidget))
	UStartupMenu*	StartupMenu;

	UPROPERTY(meta=(BindWidget))
	UHostGameMenu*	HostGameMenu;

	
	UFUNCTION()
	void OnStartupMenuButtonClicked_Callback(EButtonClicked ClickedOn);
	
	UFUNCTION()
	void OnHostGameMenuSettingAccepted_Callback(FHostGameSessionSettings Settings);
	
	UFUNCTION()
	void BackButtonPressed_Callback();
	
	ISessionController* SessionController;
};
