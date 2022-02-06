// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickUpActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerUpActor;

UCLASS()
class COOPGAME_API ASPickUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickUpActor();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
		USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
		UDecalComponent* DecalComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "PowerUpActor")
	TSubclassOf<ASPowerUpActor> PowerUpClass;

	ASPowerUpActor* PowerUpActorInstance;

	UFUNCTION()
		void SpawnPowerUpActor();

	UPROPERTY(EditInstanceOnly, Category = "PowerUpActor")
		float ReSpawnActorDuration;

	FTimerHandle TimerHandle_RespawnActor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
