// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/DoorBase.h"

#include "Gameplay/LeverBase.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogDoor)

// Sets default values
ADoorBase::ADoorBase()
{
	// We don't need this actor to tick.
	PrimaryActorTick.bCanEverTick = false;

	// Networking
	bReplicates = true;

	// Defaults
	DoorMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Door Mesh");
	DoorMeshComponent->SetupAttachment(RootComponent);
	DoorState = EDoorState::DoorClosed;
}

// Called when the game starts or when spawned
void ADoorBase::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		if (LeversToBeBounded.Num() > 0)
		{
			for (const auto& Lever : LeversToBeBounded)
			{
				Lever->OnLeverStateChanged.AddDynamic(this, &ADoorBase::OnLeverStateChanged);
				UE_LOG(LogDoor, Log, TEXT("%s has been bound to %s"), *this->GetName(), *Lever->GetName())
			}
		}
		else
		{
			DoorState = EDoorState::DoorInvalid;
			UE_LOG(LogDoor, Warning, TEXT("Actor %s could not be bound to a lever! Its new State is DoorInvalid"),
			       *this->GetName())
		}
	}
}

void ADoorBase::OnRep_DoorState()
{
	PlayDoorStateChangedEffects(DoorState);
	GEngine->AddOnScreenDebugMessage(uint64(this), 3.0f, FColor::Silver, TEXT("Client_  Door State Change"));
}

void ADoorBase::OnLeverStateChanged(TEnumAsByte<ELeverState> State)
{
	if (GetLocalRole() != ROLE_Authority || State == ELeverState::LeverInvalid)
		return;

	DoorState = State == ELeverState::LeverOpened ? EDoorState::DoorOpened : EDoorState::DoorClosed;
	GEngine->AddOnScreenDebugMessage(uint64(this), 2.0f, FColor::Emerald, TEXT("Door state changed!"));

	if(GetNetMode() == NM_ListenServer)
		PlayDoorStateChangedEffects(DoorState);
}

void ADoorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoorBase, DoorState);
}
