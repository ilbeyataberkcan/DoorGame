// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgets/Modals/SessionFoundModal.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/ProgressBar.h"


bool USessionFoundModal::Initialize()
{
	bool bIsSuccess = true;
	bIsSuccess &= Super::Initialize();
	if(!bIsSuccess)
		return false;

	SetModalContext("Session Found");
	return bIsSuccess;
}


void USessionFoundModal::SetSessionName(const FString& Name) const
{
	SessionName->SetText(FText::FromString(Name));
}

void USessionFoundModal::SetAvailableSlots(const int32& NumberOfAvailableSlots, const int32& MaxNumberOfPlayers) const
{
	const FText& AvailableSlotText = FText::Format(FTextFormat::FromString("Slots: {0}/{1}"), NumberOfAvailableSlots, MaxNumberOfPlayers);
	AvailableSlots->SetText(AvailableSlotText);
}

void USessionFoundModal::StartTimer()
{
	GetWorld()->GetTimerManager().SetTimer(SessionJoinTimeoutTimerHandle, this, &USessionFoundModal::TimeoutSessionModal, SessionModalTimeoutTimer);
	Timer->PercentDelegate.BindUFunction(this, "GetRemainingTime");
	Timer->SynchronizeProperties();
}

void USessionFoundModal::SetSessionTimeoutTimerDuration(const float& NewDuration)
{
	// Don't allow the timer to timeout before minimum of 3 seconds
	SessionModalTimeoutTimer = NewDuration < 3.0f ? 3.0f : NewDuration;
}

void USessionFoundModal::TimeoutSessionModal()
{
	OnModalInteracted.Broadcast(this, Aborted);	
	GetWorld()->GetTimerManager().ClearTimer(SessionJoinTimeoutTimerHandle);
}

float USessionFoundModal::GetRemainingTime()
{
	const float RemainingTime = GetWorld()->GetTimerManager().GetTimerRemaining(SessionJoinTimeoutTimerHandle);
	
	// reversed remaining time so the progress bar gets empty instead of filled
	return (FMath::Max(0.00001f, RemainingTime) / SessionModalTimeoutTimer);
}

