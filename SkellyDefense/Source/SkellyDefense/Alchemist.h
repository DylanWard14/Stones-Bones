// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SkellyDefenseCharacter.h"
#include "Alchemist.generated.h"

/**
 * 
 */
UCLASS()
class SKELLYDEFENSE_API AAlchemist : public ASkellyDefenseCharacter
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Replicated)
	float CoolDown;

	UPROPERTY(EditAnywhere, Replicated)
	float CoolDownTimer;


	UPROPERTY(EditAnywhere, Replicated)
	bool FirstAbilityActive = false;

	UFUNCTION(Reliable, Server, WithValidation)
	void FirstAbilityActivate();
	void FirstAbilityActivate_Implementation();
	bool FirstAbilityActivate_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void AfterFire();
	void AfterFire_Implementation();
	bool AfterFire_Validate();

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASkellyDefenseProjectile> NormalProjectile;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASkellyDefenseProjectile> AbilityProjectile;

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	
};
