// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDoor, Log, All)


UENUM(BlueprintType)
enum EDoorState
{
	DoorInvalid UMETA(DisplayName = "State value is DoorInvalid!"),
	DoorOpened UMETA(DisplayName = "Door Opened"),
	DoorClosed UMETA(DisplayName = "Door Closed")
};

class ALeverBase;
enum ELeverState : int;

UCLASS()
class MYPROJECT_API ADoorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoorBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_DoorState();

	UFUNCTION()
	void OnLeverStateChanged(TEnumAsByte<ELeverState> State);

	// Play animations and VFX when DoorState Replicated
	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void PlayDoorStateChangedEffects(const TEnumAsByte<EDoorState>& State);

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
	USkeletalMeshComponent* DoorMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TArray<ALeverBase*> LeversToBeBounded;

	// Delegates are called on the server. So to be able to Play Effects in clients We need to use Replicated using flag.
	UPROPERTY(ReplicatedUsing=OnRep_DoorState, EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
	TEnumAsByte<EDoorState> DoorState;
};
