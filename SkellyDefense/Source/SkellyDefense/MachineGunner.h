// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SkellyDefenseCharacter.h"
#include "MachineGunner.generated.h"

/**
 * 
 */
UCLASS()
class SKELLYDEFENSE_API AMachineGunner : public ASkellyDefenseCharacter
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere)
	float NormalFireDelay = FireDelay;

	UPROPERTY(EditAnywhere)
	float RageFireDelay = FireDelay / 2;

	UPROPERTY(EditAnywhere, Replicated)
	float CoolDown;

	UPROPERTY(EditAnywhere, Replicated)
	float CoolDownTimer;

	UPROPERTY(EditAnywhere, Replicated)
	bool RageActive = false;

	UFUNCTION(Reliable, Server, WithValidation)
	void ActivateRage();
	void ActivateRage_Implementation();
	bool ActivateRage_Validate();

	UPROPERTY(EditAnywhere, Replicated)
	float TimeAbilityIsActive;

	UPROPERTY(EditAnywhere, Replicated)
	float AbilityTimer;

protected:
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};
