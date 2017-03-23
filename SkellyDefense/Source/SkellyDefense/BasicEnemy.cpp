// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "BasicEnemy.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "SkellyDefenseGameMode.h"


// Sets default values
ABasicEnemy::ABasicEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasicEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasicEnemy::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (HasAuthority())
	{
		if (Health <= 0)
		{
			
			ASkellyDefenseGameMode* TheGameMode = Cast<ASkellyDefenseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			TheGameMode->CalculateEnemiesRemaining(-1);
			
			UE_LOG(LogTemp, Warning, TEXT("Enemies Remaining = %i"), TheGameMode->EnemiesRemaining)
			Destroy();
		}
	}

}

// Called to bind functionality to input
void ABasicEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABasicEnemy::OnTakeDamage(float Damage)
{
	if (HasAuthority())
	{
		Health = Health - Damage;
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not Authority"));
	}
}

float ABasicEnemy::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	OnTakeDamage(DamageAmount);
	return DamageAmount;
}

// needed to replicate variables
void ABasicEnemy::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ABasicEnemy, Health);
}

