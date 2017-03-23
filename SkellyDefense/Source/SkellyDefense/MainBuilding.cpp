// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "MainBuilding.h"
#include "Net/UnrealNetwork.h"
#include "SkellyDefenseGameMode.h"

// Sets default values
AMainBuilding::AMainBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainBuilding::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	//CheckBuildingHealth();
}

//passess the amount of damage taken to the OnTakeDamage Function
float AMainBuilding::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	OnTakeDamage(DamageAmount);
	return DamageAmount;
}

// what happens when this object takes damage
void AMainBuilding::OnTakeDamage(float Damage)
{
	if (HasAuthority()) // is this the server
	{
		Health = Health - Damage; // calculate health
		CheckBuildingHealth();
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not Authority"));
	}
}

// checks the health of the building and changes the model accordingly
void AMainBuilding::CheckBuildingHealth()
{
	if (Health < 100 && Health >= 75)
	{
		//show 100% health model
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Show 100% model"));
	}

	else if (Health < 75 && Health >= 50)
	{
		// show 75% model
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Show 75% model"));
	}
	else if (Health < 50 && Health >= 25)
	{
		// show 50% model
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Show 50% model"));
	}
	else if (Health < 25 && Health > 0)
	{
		//show 25% model
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Show 25% model"));
	}
	else if (Health <= 0)
	{
		//GAME OVER!
		ASkellyDefenseGameMode* TheGameMode = Cast<ASkellyDefenseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		TheGameMode->GameOver = true;
		this->SetActorHiddenInGame(true);
	}
}

// needed to replicate variables
void AMainBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AMainBuilding, Health);
}

