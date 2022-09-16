// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgets/Modals/ModalBase.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

bool UModalBase::Initialize()
{
	bool bIsSuccess = true;
	bIsSuccess &= Super::Initialize();
	if(!bIsSuccess)
		return false;

	AcceptButton->OnClicked.AddDynamic(this, &UModalBase::OnAcceptButtonClicked_Callback);
	CancelButton->OnClicked.AddDynamic(this, &UModalBase::OnCancelButtonClicked_Callback);
	return bIsSuccess;
}

void UModalBase::SetModalContext(const FString& Context)
{
	if(!ensure(Context != ""))
	{
		UE_LOG(LogTemp, Warning, TEXT("Modal Context is not provided."))
		ModalContext->SetText(FText::FromString("Context Not Provided"));
		return;
	}

	ModalContext->SetText(FText::FromString(Context));
}


void UModalBase::OnAcceptButtonClicked_Callback()
{
	OnModalInteracted.Broadcast(this, Accepted);
	this->RemoveFromParent();
}

void UModalBase::OnCancelButtonClicked_Callback()
{
	this->RemoveFromParent();
	OnModalInteracted.Broadcast(this, Canceled);
}
