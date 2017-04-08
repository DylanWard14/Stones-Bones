// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "Turret.h"
#include "Engineer.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurret::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATurret::SetOwningPlayer(AActor* player)
{
	OwningPlayer = Cast<AEngineer>(player);
	UE_LOG(LogTemp, Warning, TEXT("setting owner %s"), *OwningPlayer->GetName());
}

