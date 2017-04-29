// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "Alchemist.h"
#include "Net/UnrealNetwork.h"

void AAlchemist::BeginPlay()
{
	Super::BeginPlay();
}

void AAlchemist::Tick(float DeltaTime)
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

void AAlchemist::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &AAlchemist::FirstAbilityActivate);
	PlayerInputComponent->BindAction("BeginFire", IE_Pressed, this, &AAlchemist::AfterFire);
}

void AAlchemist::FirstAbilityActivate_Implementation()
{
	if (CoolDownTimer <= 0 && FirstAbilityActive == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pressed"))
		FirstAbilityActive = true;
		CoolDownTimer = CoolDown;
	}
}

bool AAlchemist::FirstAbilityActivate_Validate()
{
	return true;
}

void AAlchemist::AfterFire_Implementation()
{
	if (FirstAbilityActive)
	{
		FirstAbilityActive = false;
	}
}

bool AAlchemist::AfterFire_Validate()
{
	return true;
}

void AAlchemist::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AAlchemist, FirstAbilityActive);
	DOREPLIFETIME(AAlchemist, CoolDownTimer);
	DOREPLIFETIME(AAlchemist, CoolDown);
}



