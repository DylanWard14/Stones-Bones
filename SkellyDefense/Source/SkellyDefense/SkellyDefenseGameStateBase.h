// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "SkellyDefenseGameStateBase.generated.h"

UCLASS()
class SKELLYDEFENSE_API ASkellyDefenseGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	void SetIsWaveActive(bool newActive);
	void AddEnemiesRemaining(int32 Amount);
	void SetWaveDelay(float Delay);
	void SetMaxWaves(int32 Waves);
	void SetCurrentWave(int32 Wave);

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool GameOver = false;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	bool IsWaveActive() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetEnemiesRemaining() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	float GetWaveDelay() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void GetWaves(int32& Max, int32& Current) const;


	int32 GetMaxWaves() const;
	UFUNCTION(BlueprintCallable, Category = "Wave")
	int32 GetCurrentWave() const;

	UPROPERTY(Replicated)
	int32 EnemiesRemaining;

	UPROPERTY(Replicated)
	int32 CurrentWave;


private:



	UPROPERTY(Replicated)
	uint32 bIsWaveActive : 1;

	UPROPERTY(Replicated)
	float WaveDelay;

	UPROPERTY(Replicated)
	int32 MaxWaves;


	
};
