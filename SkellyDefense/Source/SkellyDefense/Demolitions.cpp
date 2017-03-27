// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "Demolitions.h"
#include "Net/UnrealNetwork.h"

void ADemolitions::BeginPlay()
{
	Super::BeginPlay();
}

void ADemolitions::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CoolDownTimer > 0 && !FirstAbilityActive)
	{
		CoolDownTimer -= DeltaTime;
	}
	
	if (FirstAbilityActive)
	{
		ProjectileClass = AbilityProjectile;
	}
	else if (!FirstAbilityActive)
	{
		ProjectileClass = NormalProjectile;
	}

}

void ADemolitions::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &ADemolitions::FirstAbilityActivate);
	PlayerInputComponent->BindAction("BeginFire", IE_Pressed, this, &ADemolitions::AfterFire);
}

void ADemolitions::FirstAbilityActivate_Implementation()
{
	if (CoolDownTimer <= 0 && FirstAbilityActive == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pressed"))
		FirstAbilityActive = true;
		CoolDownTimer = CoolDown;
	}
}

bool ADemolitions::FirstAbilityActivate_Validate()
{
	return true;
}

void ADemolitions::AfterFire_Implementation()
{
	if (FirstAbilityActive)
	{
		FirstAbilityActive = false;
	}
}

bool ADemolitions::AfterFire_Validate()
{
	return true;
}

void ADemolitions::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ADemolitions, FirstAbilityActive);
	DOREPLIFETIME(ADemolitions, CoolDownTimer);
	DOREPLIFETIME(ADemolitions, CoolDown);
}


