// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeWeapon.h"
#include "ProjectileActor.h"
#include "DrawDebugHelpers.h"
#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
AGrenadeWeapon::AGrenadeWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling GrenadeWeapon Construction Function"));
}


void AGrenadeWeapon::Fire()
{
	

	UE_LOG(LogTemp, Warning, TEXT("Calling GrenadeWeapon Fire"));

	if (ProjectileClass)
	{
		ASCharacter* WeaponOwner = Cast<ASCharacter>(GetOwner());

		// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
		// Use controller rotation which is our view direction in first person
		FVector MuzzleLocation = MeshComp->GetSocketLocation(ShootEffectSocketName);
		FRotator MuzzleRotation = WeaponOwner->GetController()->GetControlRotation();

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = WeaponOwner;
		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);

		UE_LOG(LogTemp, Warning, TEXT("Spawning Projectile Actor"));
	}
}
