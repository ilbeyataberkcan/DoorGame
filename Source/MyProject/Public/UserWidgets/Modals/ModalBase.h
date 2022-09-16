// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModalBase.generated.h"

UENUM()
enum EModalResult
{
	Aborted,
	Accepted,
	Canceled
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnModalInteracted, class UModalBase*, EModalResult);

/**
 * 
 */
UCLASS()
class MYPROJECT_API UModalBase : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnModalInteracted OnModalInteracted;

	/*
	 * Internally converted to FText. Use FText for convenience of use.
	 */
	void SetModalContext(const FString& Context);

	virtual bool Initialize() override;

private:

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ModalContext;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* AcceptButton;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* CancelButton;

	
	UFUNCTION()
	void OnAcceptButtonClicked_Callback();
	
	UFUNCTION()
	void OnCancelButtonClicked_Callback();
};
