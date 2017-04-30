// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "Engineer.h"
#include "Turret.h"

void AEngineer::BeginPlay()
{
	Super::BeginPlay();

	TurretsSpawned = 0;
}

void AEngineer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health > 0 && !canDestroyTurrets)
	{
		canDestroyTurrets = true;
	}
	else if (Health <= 0 && canDestroyTurrets)
	{
		for (int i = 0; i < SpawnedTurrets.Num(); i++)
		{
			SpawnedTurrets[i]->Destroy();
		}
		SpawnedTurrets.Empty();
		
		canDestroyTurrets = false;
		TurretsSpawned = 0;
	}
}

void AEngineer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AEngineer::FirstAbility);
}

void AEngineer::FirstAbility_Implementation()
{
	if (TurretsSpawned < MaxAmountOfTurrets)
	{
		//spawn a turret
		TurretsSpawned++;
		FVector spawnLocation = GetActorLocation() + (GetActorForwardVector() * 200);
		AActor* spawnedActor = GetWorld()->SpawnActor<ATurret>(turret, spawnLocation, GetActorRotation());
		SpawnedTurrets.Add(spawnedActor);
		ATurret* spawnedTurret = Cast<ATurret>(spawnedActor);
		spawnedTurret->SetOwningPlayer(this);
	}
}

bool AEngineer::FirstAbility_Validate()
{
	return true;
}

