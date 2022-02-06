// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoopGameState.h"
#include "Net/UnrealNetwork.h"

void ASCoopGameState::OnRep_WaveState(EWaveState OldState)
{
	OnWaveStateChanged(OldState, WaveState);
}

void ASCoopGameState::SetWaveState(EWaveState NewState)
{
	if (HasAuthority())
	{
		EWaveState OldState = WaveState;
		WaveState = NewState;
		OnRep_WaveState(OldState);
	}
}

void ASCoopGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASCoopGameState, WaveState);
}
