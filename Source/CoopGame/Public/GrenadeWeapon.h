// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "GrenadeWeapon.generated.h"


class AProjectileActor;
/**
 * 
 */
UCLASS()
class COOPGAME_API AGrenadeWeapon : public ASWeapon
{
	GENERATED_BODY()
public:
		AGrenadeWeapon();

protected:
		//UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AProjectileActor> ProjectileClass;
};
