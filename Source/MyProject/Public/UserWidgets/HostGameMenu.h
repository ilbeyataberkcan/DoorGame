// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HostGameMenu.generated.h"

USTRUCT()
struct MYPROJECT_API FHostGameSessionSettings
{
	GENERATED_BODY()

	int32 MaxNumberOfPlayers = 2;
	FName SessionName = "MySession";
	bool bIsLAN = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHostingSettingsAccepted, FHostGameSessionSettings, SessionSettings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackButtonClicked);

/*
 * 
 */
UCLASS()
class MYPROJECT_API UHostGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnHostingSettingsAccepted	OnHostingSettingsAccepted;
	FOnBackButtonClicked		OnBackButtonClicked;

protected:
	bool Initialize() override;
	
private:


	UPROPERTY(meta=(BindWidget))
	class UEditableTextBox* ServerName_Input;
	
	UPROPERTY(meta=(BindWidget))
	class USpinBox* MaxNumberOfPlayers;
	
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* bIsLAN;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* CreateGameButton;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* BackButton;

	UFUNCTION()
	void OnCreateGameButtonClicked_Callback();

	UFUNCTION()
	void OnBackButtonClicked_Callback();
	
};
