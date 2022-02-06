// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SCoopGameState.generated.h"


UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,

	GenerationInProgress,

	WaveComplete,

	GameOver,
};
/**
 * 
 */
UCLASS()
class COOPGAME_API ASCoopGameState : public AGameStateBase
{
	GENERATED_BODY()
protected:
	UFUNCTION()
		void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent)
		void OnWaveStateChanged(EWaveState OldState, EWaveState NewState);
	
public:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

	UFUNCTION()
		void SetWaveState(EWaveState NewState);

};
