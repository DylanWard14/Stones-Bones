// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MainBuilding.generated.h"

UCLASS()
class SKELLYDEFENSE_API AMainBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainBuilding();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health, Replicated)
	int32 Health = 100; //TODO find better value

protected:
	
	void CheckBuildingHealth(); // checks the health of the building and swaps the model accordingly

private:

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)override;

	UFUNCTION()
	void OnTakeDamage(float Damage);
	
};
