// Fill out your copyright notice in the Description page of Project Settings.

#include "SkellyDefense.h"
#include "SkellyDefenseGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "SkellyDefenseGameMode.h"

void ASkellyDefenseGameStateBase::SetIsWaveActive(bool newActive)
{
	bIsWaveActive = newActive;
}

void ASkellyDefenseGameStateBase::AddEnemiesRemaining(int32 Amount)
{
	EnemiesRemaining += Amount;
}

void ASkellyDefenseGameStateBase::SetWaveDelay(float Delay)
{
	WaveDelay = Delay;
}

void ASkellyDefenseGameStateBase::SetMaxWaves(int32 Waves)
{
	MaxWaves = Waves;
}

void ASkellyDefenseGameStateBase::SetCurrentWave(int32 Wave)
{
	CurrentWave = Wave;
}

bool ASkellyDefenseGameStateBase::IsWaveActive() const
{
	return bIsWaveActive;
}

int32 ASkellyDefenseGameStateBase::GetEnemiesRemaining() const
{
	//ASkellyDefenseGameMode* TheGameMode = Cast<ASkellyDefenseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	return EnemiesRemaining;
	//return TheGameMode->EnemiesRemaining;
}

float ASkellyDefenseGameStateBase::GetWaveDelay() const
{
	return WaveDelay;
}

void ASkellyDefenseGameStateBase::GetWaves(int32 & Max, int32 & Current) const
{
	Max = MaxWaves;
	Current = CurrentWave;
}

int32 ASkellyDefenseGameStateBase::GetMaxWaves() const
{
	return MaxWaves;
}

int32 ASkellyDefenseGameStateBase::GetCurrentWave() const
{
	return CurrentWave;
}

// this function replicatese these properties
void ASkellyDefenseGameStateBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASkellyDefenseGameStateBase, bIsWaveActive);
	DOREPLIFETIME(ASkellyDefenseGameStateBase, EnemiesRemaining);
	DOREPLIFETIME(ASkellyDefenseGameStateBase, CurrentWave);
	DOREPLIFETIME(ASkellyDefenseGameStateBase, MaxWaves);
	DOREPLIFETIME(ASkellyDefenseGameStateBase, WaveDelay);
	DOREPLIFETIME(ASkellyDefenseGameStateBase, GameOver);
}

