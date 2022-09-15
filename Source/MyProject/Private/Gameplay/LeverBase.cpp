// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/LeverBase.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ALeverBase::ALeverBase()
{
	// We don't need this actor tick
	PrimaryActorTick.bCanEverTick = false;

	// Networking
	bReplicates = true;

	// Defaults
	LeverState = LeverClosed;
	LeverMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Lever Mesh"));
	RootComponent = LeverMeshComponent;
}

// Called when the game starts or when spawned
void ALeverBase::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority)
		LeverState = OnLeverStateChanged.IsBound() ? LeverClosed : LeverInvalid;
}

void ALeverBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALeverBase, LeverState);
}

void ALeverBase::OnInteraction_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		HandleLeverSwitched();
	}
}

void ALeverBase::OnRep_LeverState()
{
	PlayLeverInteractionEffects(LeverState);
}

void ALeverBase::HandleLeverSwitched()
{
	LeverState = LeverState == LeverOpened ? LeverClosed : LeverOpened;

	// Broadcast the event only on the server. This is for game logic so Clients doesn't need to know about it.
	OnLeverStateChanged.Broadcast(LeverState);
}
