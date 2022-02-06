// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AIExplodeTracker.generated.h"

class USHealthComponent;
class UMaterialInstanceDynamic;
class USphereComponent;
class UAudioComponent;
UCLASS()
class COOPGAME_API AAIExplodeTracker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIExplodeTracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
		USphereComponent* ExplosionDetectionSphereComp;

	FVector GetNextNaviPoint();

	void Explode();

	void SelfDamage();

	void PlayExplodeEffect();

	UFUNCTION()
	void OnTrackerHealthChanged(USHealthComponent* HealthComponent, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
		void OnTrackerNearCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	FTimerHandle ExplosionTimerHandle;

	bool IsTrackerExploded;

	// This is used to control timer would be triggerd once;
	bool IsNearTimerTriggerd;

	//TSubclassOf<UDamageType> DamageType;

	FVector NextPointLocation;

	FVector ForceDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Navi")
	float DistanceTolarence;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
		USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Navi")
	bool IsVelChanged;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystem* ExplodeEffect;

	UMaterialInstanceDynamic* MeshMaterialInst;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* RollingSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* ExplodeWarningSound;

	FTimerHandle TimerHandle_RestartSearchPoint;

	void RefreshTargetPoint();


};
