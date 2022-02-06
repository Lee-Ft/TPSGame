// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpActor.h"

// Sets default values
ASPowerUpActor::ASPowerUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PowerUpInterval = 1.f;
	TotalPowerUpTicks = 5;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPowerUpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerUpActor::ActorBeenActivated(AActor* OtherActor)
{
	if (PowerUpInterval > 0)
	{
		GetWorldTimerManager().SetTimer(TimeHandle_PowerUp, this, &ASPowerUpActor::TickPowerUp, PowerUpInterval, true);
		OnActivated(OtherActor);
	}
	
}

// Called every frame
void ASPowerUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASPowerUpActor::TickPowerUp()
{

	CurrentPowerUpTicks++;

	OnPowerUpTicked();
	if (CurrentPowerUpTicks >= TotalPowerUpTicks)
	{
		GetWorldTimerManager().ClearTimer(TimeHandle_PowerUp);
		OnExpired();
	}

}


