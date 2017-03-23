// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BasicEnemy.generated.h"

UCLASS()
class SKELLYDEFENSE_API ABasicEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(BlueprintReadWrite, Replicated)
	float Health = 100;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)override;

	UFUNCTION()
	void OnTakeDamage(float Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void AttackNorth();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void AttackEast();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void AttackSouth();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void AttackWest();
	
	UPROPERTY(BlueprintReadOnly, Category = "AI")
	int32 WallIndex;
};
