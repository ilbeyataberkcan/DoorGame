// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartupMenu.generated.h"

UENUM()
enum class EButtonClicked : uint8
{
	None,
	HostGame,
	FindGame,
	QuitGame
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuButtonClicked, EButtonClicked, ButtonClicked);

/**
 * 
 */
UCLASS()
class MYPROJECT_API UStartupMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnMenuButtonClicked OnMenuButtonClicked;

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta=(BindWidget))
	class UButton* HostGame;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* FindGame;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* QuitGame;

	UFUNCTION()
	void OnHostGameButtonClicked_Callback();
	UFUNCTION()
	void OnFindGameButtonClicked_Callback();
	UFUNCTION()
	void OnQuitGameButtonClicked_Callback();
	
};
