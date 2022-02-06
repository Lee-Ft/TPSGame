// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerUpActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUpActor();

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpInterval;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int32 TotalPowerUpTicks;

	bool IsActorActivated;

	int32 CurrentPowerUpTicks;

	FTimerHandle TimeHandle_PowerUp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
		void OnActivated(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
		void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
		void OnPowerUpTicked();

	UFUNCTION()
		void TickPowerUp();

	void ActorBeenActivated(AActor* OtherActor);
};
