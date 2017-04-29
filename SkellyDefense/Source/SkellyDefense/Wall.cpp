// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "Wall.h"
#include "Net/UnrealNetwork.h"
#include "SkellyDefenseGameMode.h"

// Sets default values
AWall::AWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWall::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<ASkellyDefenseGameMode>(GetWorld()->GetAuthGameMode());
	
}

// Called every frame
void AWall::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CheckDestroyWall();

}
// this should be a server only function but as SetActorEnableCollision is not replicated it must happen on server and client
void AWall::CheckDestroyWall()
{
	if (Health <= 0)
	{
		this->SetActorEnableCollision(false);
		this->SetActorHiddenInGame(true);
		
		if (HasAuthority())
		{
			if (GameMode->WallDestroyed == false)
				GameMode->WallDestroyed = true;
		}
		//Destroy();
	}
}

//bool AWall::CheckDestroyWall_Validate()
//{
//	return true;
//}

float AWall::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	OnTakeDamage(DamageAmount);
	return DamageAmount;
}

void AWall::OnTakeDamage(float Damage)
{
	if (HasAuthority()) // is this the server
	{
		Health = Health - Damage; // calculate health
	}

	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not Authority"));
	}

	CheckDestroyWall();
}

// needed to replicate variables
void AWall::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AWall, Health);
}