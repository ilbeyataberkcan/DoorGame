// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DoorGameCharacter.generated.h"


UCLASS(config=Game)
class ADoorGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Player, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PlayerNameTagComponent;

public:
	ADoorGameCharacter();

	virtual void Tick(float DeltaSeconds) override;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);
	
	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);


	// Server calls should be validated. For this case there is no parameters we would use to determine whether or not
	// should we disconnect, so, it is always true
	UFUNCTION(Server, Reliable, WithValidation, Category="Interaction")
	void Interact();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
private:
	/**
	 * @brief Interact with the given object via IInteractable Interface.
	 * Check if the actor passed to parameter implements the Interface Internally. 
	 * @param Interactable Object that implements IInteractable Interface
	 */
	void InteractWithObject(AActor* Interactable) const;


	/**
	 * @brief Line trace for objects of custom Trace Channel "Interactable".
	 * @param HitActor Pointer to the actor if the hit was successfull. nullptr otherwise
	 */
	AActor* LineTraceForInteractables();


	/**
	 * @brief Runs the logic of interaction. Called by the Interact function. 
	 */
	void HandleInteraction();
};
