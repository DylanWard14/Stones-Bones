// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "SkellyDefenseGameStateBase.h"
#include "SkellyDefenseGameMode.generated.h"


class ABasicEnemy;
class AWall;

UCLASS(minimalapi)
class ASkellyDefenseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASkellyDefenseGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	TArray<AActor*> Spawns;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ABasicEnemy>> EnemyBP;

	FTimerHandle SpawnTimerHandle;

	void CalculateEnemiesRemaining (float Amount);

	void CheckIncreaseWave();

	void SpawnEnemy();

	int EnemiesSpawned;

	int CurrentWave;

	int EnemiesToSpawn;

	int EnemiesRemaining = 11;
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHUD();

	UPROPERTY(BlueprintReadWrite)
	TArray <AWall*> NorthWalls;

	UPROPERTY(BlueprintReadWrite)
	TArray <AWall*> EastWalls;

	UPROPERTY(BlueprintReadWrite)
	TArray <AWall*> WestWalls;

	UFUNCTION(BlueprintCallable, Category = "Wall")
	bool CheckArrayIndexNull(TArray<AActor*> ThisArray, int32 Index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool GameOver = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Ready = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WallDestroyed = false;

protected: 
	virtual void InitGameState() override;

	void CheckGameOver();

private:
	class ASkellyDefenseGameStateBase* TheGameState;
};



