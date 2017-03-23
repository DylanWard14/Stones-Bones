// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "MachineGunner.h"
#include "Net/UnrealNetwork.h"

void AMachineGunner::BeginPlay()
{
	Super::BeginPlay();
	NormalFireDelay = FireDelay;
	RageFireDelay = NormalFireDelay / 2;
}

void AMachineGunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CoolDownTimer > 0)
	{
		CoolDownTimer -= DeltaTime;
	}

	if (RageActive == true)
	{
		AbilityTimer -= DeltaTime;
		if (AbilityTimer <= 0)
		{
			AbilityTimer = 0;
			RageActive = false;
			FireDelay = NormalFireDelay;
		}
	}
}

void AMachineGunner::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AMachineGunner::ActivateRage);
}

void AMachineGunner::ActivateRage_Implementation()
{
	if (CoolDownTimer <= 0 && RageActive == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pressed"))
		RageActive = true;
		CoolDownTimer = CoolDown;
		AbilityTimer = TimeAbilityIsActive;
		FireDelay = RageFireDelay;
	}
}

bool AMachineGunner::ActivateRage_Validate()
{
	return true;
}

void AMachineGunner::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AMachineGunner, RageActive);
	DOREPLIFETIME(AMachineGunner, CoolDownTimer);
	DOREPLIFETIME(AMachineGunner, CoolDown);
	DOREPLIFETIME(AMachineGunner, TimeAbilityIsActive);
	DOREPLIFETIME(AMachineGunner, AbilityTimer);
}
