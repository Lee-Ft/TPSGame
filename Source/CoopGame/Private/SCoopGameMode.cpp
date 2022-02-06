// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoopGameMode.h"
#include "TimerManager.h"
#include "SHealthComponent.h"
#include "SCoopGameState.h"
#include "SPlayerState.h"
ASCoopGameMode::ASCoopGameMode()
{
	SpawnWavePeriod = 7.f;
	CurrentWave = 0, TargetWave = 10;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 3.f;

	GameStateClass = ASCoopGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASCoopGameMode::StartPlay()
{
	Super::StartPlay();
	PrepareForNextWave();
	GetWorldTimerManager().SetTimer(TimerHandle_CheckWaveState, this, &ASCoopGameMode::CheckWaveState, 3.f, true, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_CheckPlayerAlive, this, &ASCoopGameMode::CheckIfAnyPlayerAlive, 1.f, true, 0.0f);

	SetWaveState(EWaveState::WaitingToStart);
}



void ASCoopGameMode::StartWave()
{
	SetWaveState(EWaveState::GenerationInProgress);
	CurrentWave++;
	NumberOfBotInWave = CurrentWave;
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnActorsInWave, this, &ASCoopGameMode::CallSpawnTrackerBot, 1.f, true, 0.0f);
}

void ASCoopGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnActorsInWave);
	SetWaveState(EWaveState::WaveComplete);
}

void ASCoopGameMode::PrepareForNextWave()
{
	if (CurrentWave < TargetWave)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PrepareWaveDelay, this, &ASCoopGameMode::StartWave, SpawnWavePeriod, false, 0.0f);
	}
	else
	{
		// Has reached target wave, end the game, player pass the level
		EndCheck();
		GameOver(true);
	}
	
}

void ASCoopGameMode::CallSpawnTrackerBot()
{
	SpawnTrackerBot();
	NumberOfBotInWave--;
	if (NumberOfBotInWave <= 0)
	{
		EndWave();
	}
}

void ASCoopGameMode::CheckWaveState()
{

	if (NumberOfBotInWave || GetWorldTimerManager().IsTimerActive(TimerHandle_PrepareWaveDelay))
	{
		return;
	}
	bool IsHaveBotAlive = false;
	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; it++)
	{
		APawn* TestPawn = it->Get();

		if (TestPawn == nullptr || TestPawn->IsLocallyControlled())
		{
			continue;
		}
		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp->GetHealth() > 0)
		{
			IsHaveBotAlive = true;
			break;
		}
		
	}
	if (!IsHaveBotAlive)
	{
		PrepareForNextWave();
	}
	
}

void ASCoopGameMode::EndCheck()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_CheckWaveState);
}

void ASCoopGameMode::CheckIfAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		APlayerController* PC = it->Get();

		APawn* TestPawn = PC->GetPawn();
		if (TestPawn)
		{
			USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0)
			{
				return;
			}
		}

	}
	GameOver(false);
}

void ASCoopGameMode::GameOver(bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Error, TEXT("You Have Passed the Level!!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("All Players Are Dead!!"));
	}

	SetWaveState(EWaveState::GameOver);
}

void ASCoopGameMode::SetWaveState(EWaveState NewState)
{
	ASCoopGameState* GS = GetGameState<ASCoopGameState>();

	if (ensure(GS))
	{
		GS->SetWaveState(NewState);
	}
}
