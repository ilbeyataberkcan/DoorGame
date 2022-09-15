// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/SessionController.h"
#include "WidgetManagerSubsystem.generated.h"

class FOnlineSessionSearchResult;

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

private:

	UFUNCTION()
	void OnSessionCreated_Callback();
	void OnSessionFound_Callback(const TArrayView<FOnlineSessionSearchResult>& FoundSessions, bool bIsSuccessful);
	void OnSessionJoin_Callback(FName SessionName,EOnJoinSessionCompleteResult::Type Result);
	
	TSubclassOf<class UMainMenu> MainMenuClass;
	UMainMenu* MainMenu;
	
	// Widget controller represent the Session Subsystem or any other class that handles the Online Subsystem
	ISessionController* WidgetController;

	
};
