// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "LeverBase.generated.h"

UENUM(BlueprintType)
enum ELeverState 
{
	LeverInvalid UMETA(DisplayName = "Lever LeverState LeverInvalid"),
	LeverOpened	UMETA(DisplayName = "Lever LeverOpened"),
	LeverClosed	UMETA(DisplayName = "Lever LeverClosed")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeverStateChanged, TEnumAsByte<ELeverState>, State);

UCLASS(Blueprintable)
class MYPROJECT_API ALeverBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ALeverBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// IInteractable Interface Function
	virtual void OnInteraction_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_LeverState();


	UFUNCTION(BlueprintImplementableEvent, Category = "Lever")
	void PlayLeverInteractionEffects(const TEnumAsByte<ELeverState>& State);

private:
	void HandleLeverSwitched();
	

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lever")
	USkeletalMeshComponent* LeverMeshComponent;

	UPROPERTY(BlueprintAssignable, Category = "Lever")
	FOnLeverStateChanged OnLeverStateChanged;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_LeverState, Category = "Lever")
	TEnumAsByte<ELeverState> LeverState;

};
