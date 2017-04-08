// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class AEngineer;
UCLASS()
class SKELLYDEFENSE_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AEngineer* OwningPlayer;

	UFUNCTION()
	void SetOwningPlayer(AActor* player);
	
};
