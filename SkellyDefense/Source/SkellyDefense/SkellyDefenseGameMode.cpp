// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SkellyDefense.h"
#include "SkellyDefenseGameMode.h"
#include "SkellyDefenseHUD.h"
#include "SkellyDefenseCharacter.h"
#include "EnemySpawnPoint.h"
#include "BasicEnemy.h"
#include "SkellyDefenseGameStateBase.h"
#include "Wall.h"

ASkellyDefenseGameMode::ASkellyDefenseGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASkellyDefenseHUD::StaticClass();

	GameStateClass = ASkellyDefenseGameStateBase::StaticClass();
}

void ASkellyDefenseGameMode::InitGameState()
{
	Super::InitGameState();

	TheGameState = Cast<ASkellyDefenseGameStateBase>(GameState);
}

void ASkellyDefenseGameMode::BeginPlay()
{
	Super::BeginPlay();
	UWorld* const World = GetWorld();
	Ready = false;
	WallDestroyed = false;
	if (World)
	{
		GameOver = false;
		CurrentWave = 1;
		TheGameState->CurrentWave = this->CurrentWave;
		EnemiesToSpawn = CurrentWave + 10;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPoint::StaticClass(), Spawns);
		UE_LOG(LogTemp, Warning, TEXT("Being play"));
		UE_LOG(LogTemp, Warning, TEXT("%i"), Spawns.Num());

		EnemiesSpawned = 0;
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASkellyDefenseGameMode::SpawnEnemy, 2.0f, false);
		//UpdateHUD();

		TArray<AActor*> Walls;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWall::StaticClass(), Walls);

		for (AActor* Wall : Walls)
		{
			if (Wall->ActorHasTag("NorthWall"))
			{
				AWall* ThisWall = Cast<AWall>(Wall);
				NorthWalls.Add(ThisWall);
				UE_LOG(LogTemp, Warning, TEXT("North wall added"));
			}

			else if (Wall->ActorHasTag("EastWall"))
			{
				AWall* ThisWall = Cast<AWall>(Wall);
				EastWalls.Add(ThisWall);
				UE_LOG(LogTemp, Warning, TEXT("East wall added"));
			}

			else if (Wall->ActorHasTag("WestWall"))
			{
				AWall* ThisWall = Cast<AWall>(Wall);
				WestWalls.Add(ThisWall);
				UE_LOG(LogTemp, Warning, TEXT("West wall added"));
			}

			else if (Wall->ActorHasTag("SouthWall"))
			{

			}
		}
		
	}
}



void ASkellyDefenseGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHUD();
	if (!GameOver)
	{
		CheckIncreaseWave();
	}

	CheckGameOver();

}

void ASkellyDefenseGameMode::CheckGameOver()
{
	if (GameOver)
	{
		TheGameState->GameOver = true;
		UpdateHUD();
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!!"))
	}
}

// Calls the Update HUD method on all players
void ASkellyDefenseGameMode::UpdateHUD()
{
	TArray <AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASkellyDefenseCharacter::StaticClass(), Players);
	for (AActor* Player : Players)
	{
		ASkellyDefenseCharacter* CastedPlayer = Cast<ASkellyDefenseCharacter>(Player);
		if (CastedPlayer && CastedPlayer->IsPlayerControlled())
		{
			CastedPlayer->UpdateHUD();
		}
	}
}

// calculates the enemies remaining and sends it to the game state so that HUD can tell the player
void ASkellyDefenseGameMode::CalculateEnemiesRemaining(float Amount)
{
	EnemiesRemaining+= Amount;
	TheGameState->EnemiesRemaining = EnemiesRemaining;
	//UpdateHUD();
	//TODO update hud here. this way it is called when this value is changed
}

//Spawns an enemy at a random spawn position
void ASkellyDefenseGameMode::SpawnEnemy()
{
	if (Ready && EnemiesSpawned < EnemiesToSpawn && EnemiesRemaining > 0)
	{
		int SpawnIndex = FMath::RandRange(0, (Spawns.Num() - 1));
		UE_LOG(LogTemp, Warning, TEXT("%i"), SpawnIndex);


		auto SpawnPosition = Spawns[SpawnIndex]->GetActorLocation();
		auto SpawnRotation = Spawns[SpawnIndex]->GetActorRotation();
		int EnemyToSpawn = 0;

		if (WallDestroyed) //TODO Change to allow only explosive enemies to spawn on side of destroyed wall
			EnemyToSpawn = FMath::RandRange(0, (EnemyBP.Num() - 1));
		else
			EnemyToSpawn = FMath::RandRange(0, (EnemyBP.Num() - 2));

		AActor* SpawnedEnemy = GetWorld()->SpawnActor<ABasicEnemy>(EnemyBP[EnemyToSpawn], SpawnPosition, SpawnRotation);
		ABasicEnemy* Enemy = Cast<ABasicEnemy>(SpawnedEnemy);

		if (Spawns[SpawnIndex]->ActorHasTag("North") && !Enemy->ActorHasTag("Explosive")) //TODO turn into function
		{
			Enemy->AttackNorth();
			Enemy->WallIndex = FMath::RandRange(0, (NorthWalls.Num() - 1));
		}

		else if (Spawns[SpawnIndex]->ActorHasTag("East") && !Enemy->ActorHasTag("Explosive"))
		{
			Enemy->AttackEast();
			Enemy->WallIndex = FMath::RandRange(0, (EastWalls.Num() - 1));
		}
		else if (Spawns[SpawnIndex]->ActorHasTag("West") && !Enemy->ActorHasTag("Explosive"))
		{
			Enemy->AttackWest();
			Enemy->WallIndex = FMath::RandRange(0, (WestWalls.Num() - 1));
		}
		else if (Enemy->ActorHasTag("Explosive"))
		{
			Enemy->AttackPlayer();
		}
		
		//GetWorld()->SpawnActor<ABasicEnemy>(EnemyBP, SpawnPosition, SpawnRotation);
		EnemiesSpawned++;
		UE_LOG(LogTemp, Warning, TEXT("Enemies Spawned = %i"), EnemiesSpawned);
		UpdateHUD();
		
	}
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASkellyDefenseGameMode::SpawnEnemy, 2.0f, false);
}

// checks to see if all the enemies have been killed and increases the wave accordingly
void ASkellyDefenseGameMode::CheckIncreaseWave()
{
	if (EnemiesRemaining <= 0)
	{
		CurrentWave++;
		TheGameState->CurrentWave = this->CurrentWave;
		EnemiesToSpawn = CurrentWave + 10;
		EnemiesSpawned = 0;
		CalculateEnemiesRemaining(EnemiesToSpawn);
		UpdateHUD();
		UE_LOG(LogTemp, Warning, TEXT("Current Wave: %i"), CurrentWave)
			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASkellyDefenseGameMode::SpawnEnemy, 10.0f, false);
	}
}

bool ASkellyDefenseGameMode::CheckArrayIndexNull(TArray<AActor*> ThisArray, int32 Index)
{
	if (ThisArray[Index] == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Null"))
		return true;
	}
	else
	{
		return false;
	}
}
