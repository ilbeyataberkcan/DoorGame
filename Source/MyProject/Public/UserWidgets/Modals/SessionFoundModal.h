// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidgets/Modals/ModalBase.h"
#include "SessionFoundModal.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API USessionFoundModal : public UModalBase
{
	GENERATED_BODY()

public:
	void SetSessionName(const FString& Name) const;
	// Set Available Slots Text Block
	void SetAvailableSlots(const int32& NumberOfAvailableSlots, const int32& MaxNumberOfPlayers) const;

	// Timer should be started maually after  
	void StartTimer();
	void SetSessionTimeoutTimerDuration(const float& NewDuration);

	UFUNCTION()
	void TimeoutSessionModal();

	UFUNCTION()
	float GetRemainingTime();

	virtual bool Initialize() override;

private:
	
	float SessionModalTimeoutTimer = 5.0f;
	
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* Timer;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SessionName;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* AvailableSlots;

	FTimerHandle SessionJoinTimeoutTimerHandle;
};
