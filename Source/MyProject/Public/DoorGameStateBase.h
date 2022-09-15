// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DoorGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalPlayerNameChanged, FString, NewName);

/**
 * 
 */
UCLASS(BlueprintType)
class MYPROJECT_API ADoorGameStateBase : public AGameStateBase 
{
	GENERATED_BODY()

	ADoorGameStateBase();
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadWrite)
	FString GlobalPlayerName;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Player")
	void SetGlobalPlayerName_Server(const FString& NewName);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetGlobalPlayerName(const FString& NewGlobalName);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void GetGlobalPlayerName(FString& Name);

	// IMenu Interaction Interface
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnGlobalPlayerNameChanged OnGlobalPlayerNameChanged;

};
