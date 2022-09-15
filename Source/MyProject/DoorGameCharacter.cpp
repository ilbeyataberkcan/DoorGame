// Copyright Epic Games, Inc. All Rights Reserved.

#include "DoorGameCharacter.h"

#include "PlayerNameState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/Interactable.h"
#include "Kismet/KismetMathLibrary.h"


// Custom console variable to Show Debug Lines
static TAutoConsoleVariable<bool> CVarShowTraceDebugLines(
	TEXT("p.ShowTraceDebugLines"), false, TEXT("Show debug lines for interaction ITraces"), ECVF_Cheat);

//////////////////////////////////////////////////////////////////////////
// ADoorGameCharacter

ADoorGameCharacter::ADoorGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm 

	PlayerNameTagComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player Name Tag"));
	PlayerNameTagComponent->SetupAttachment(RootComponent);
	PlayerNameTagComponent->SetDrawSize(FVector2D(250.0f, 125.0f));
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ADoorGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Make the Name Tag face the camera all the time. 
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerNameTagComponent->GetComponentLocation(), FollowCamera->GetComponentLocation());
	PlayerNameTagComponent->SetWorldRotation(LookAtRotation);
}

//////////////////////////////////////////////////////////////////////////
// Input



void ADoorGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Interaction
	PlayerInputComponent->BindAction("Interaction", IE_Released, this, &ADoorGameCharacter::Interact);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ADoorGameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ADoorGameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ADoorGameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ADoorGameCharacter::LookUpAtRate);
}

void ADoorGameCharacter::InteractWithObject(AActor* Interactable) const
{
	if (GetLocalRole() == ROLE_Authority && Interactable->Implements<UInteractable>())
	{
		IInteractable::Execute_OnInteraction(Interactable);
	}
}

AActor* ADoorGameCharacter::LineTraceForInteractables()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FHitResult Hit;
		const FVector TraceStart = FollowCamera->GetComponentLocation();
		const FVector TraceEnd = TraceStart + FollowCamera->GetForwardVector() * 5000;

		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel1);

		if (Hit.bBlockingHit)
		{
			return Hit.GetActor();
		}
	}
	return nullptr;
}

void ADoorGameCharacter::HandleInteraction()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AActor* Interactable = LineTraceForInteractables();

		if (Interactable)
		{
			InteractWithObject(Interactable);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Interactable Actor is NULLPTR"));
		}
	}
}

void ADoorGameCharacter::Interact_Implementation()
{
	HandleInteraction();
}

bool ADoorGameCharacter::Interact_Validate()
{
	return true;
}

void ADoorGameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ADoorGameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ADoorGameCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ADoorGameCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
