// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SCoopGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilledSignature, AActor*, VictimActor, AActor*, KillerActor, AController*, DamageCauser);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, float, Health, float, DeltaHealth, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

enum class EWaveState : uint8;
/**
 * 
 */
UCLASS()
class COOPGAME_API ASCoopGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASCoopGameMode();

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
		float SpawnWavePeriod;

	virtual void StartPlay() override;
	
	//virtual void Tick(float deltaTime) override;

	int32 CurrentWave;

	int32 TargetWave;

	int32 NumberOfBotInWave;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
		FOnActorKilledSignature OnActorKilled;
protected:

	UFUNCTION()
	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
		void SpawnTrackerBot();

	UFUNCTION()
	void CallSpawnTrackerBot();

	//UFUNCTION();
	void CheckWaveState();

	void EndCheck();

	FTimerHandle TimerHandle_SpawnActorsInWave;

	FTimerHandle TimerHandle_PrepareWaveDelay;

	FTimerHandle TimerHandle_CheckWaveState;

	FTimerHandle TimerHandle_CheckPlayerAlive;

	void CheckIfAnyPlayerAlive();

	void GameOver(bool Success);

	void SetWaveState(EWaveState NewState);
	
};
