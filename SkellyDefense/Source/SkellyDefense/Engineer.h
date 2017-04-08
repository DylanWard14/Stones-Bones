// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SkellyDefenseCharacter.h"
#include "Engineer.generated.h"


/**
 * 
 */
UCLASS()
class SKELLYDEFENSE_API AEngineer : public ASkellyDefenseCharacter
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AActor> turret;

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void FirstAbility();
	void FirstAbility_Implementation();
	bool FirstAbility_Validate();

	UPROPERTY(EditAnywhere)
	int32 MaxAmountOfTurrets = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TurretsSpawned;

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
};
