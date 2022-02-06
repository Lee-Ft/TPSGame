// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FVector_NetQuantize10 TraceStart;
	UPROPERTY()
		FVector_NetQuantize10 TraceEnd;

	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;
};

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		USkeletalMeshComponent* MeshComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* ShootEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FName ShootEffectSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FName TracerTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* VulnerableImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamege;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* TraceEffect;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShake> FireShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float FireSpeed;

	float TimeBetweenFire;

	float LastFireSpeed;

	void PlayFireEffect(FVector, FVector);

	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint, FVector MuzzleLocation);

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;

	UFUNCTION()
		void OnRep_HitScanTrace();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Fire();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BulletSpreadDegree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;



	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EndFire();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* FireSound;

	FTimerHandle FireBetweenShots_TimerHandle;
};
