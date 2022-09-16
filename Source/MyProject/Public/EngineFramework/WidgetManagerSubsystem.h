// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/SessionController.h"
#include "WidgetManagerSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class MYPROJECT_API UWidgetManagerSubsystem : public UGameInstanceSubsystem 
{
	GENERATED_BODY()

public:
	UWidgetManagerSubsystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void SetWidgetController(ISessionController* InController);

	UFUNCTION(BlueprintCallable, Category = "WidgetManager")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "WidgetManager")
	void ShowSessionFoundModal(class USessionFoundModal* Modal);
	
private:

	UFUNCTION()
	void OnSessionCreated_Callback();
	void OnSessionsFound_Callback(const TArrayView<class FOnlineSessionSearchResult>& Sessions, bool bIsSuccessful);
	void OnSessionJoin_Callback(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// SessionModal functions
	
	void HandleFoundSessions();
	void HandleSessionModalCancelOrTimeout();

	UFUNCTION()
	void OnSessionInteraction_Callback(class UModalBase* Menu, enum EModalResult Result);

	UFUNCTION()
	void OnQuitButtonPressed_Callback(); 
	
	TSubclassOf<class UMainMenu> MainMenuClass;
	TSubclassOf<class USessionFoundModal> SessionFoundModalClass;
	TSubclassOf<class UModalBase> QuitGameModalClass;

	UPROPERTY()
	UMainMenu* MainMenu;

	TArrayView<class FOnlineSessionSearchResult> FoundSessions;
	int CurrentSessionIndex = 0;
	
	// Widget controller represent the Session Subsystem or any other class that handles the Online Subsystem
	ISessionController* WidgetController;

	
};


