// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerNameState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API APlayerNameState : public APlayerState
{
private:
	GENERATED_BODY()

public:
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Player")
	void SetGlobalPlayerName(const FString& NewName);
	
};
