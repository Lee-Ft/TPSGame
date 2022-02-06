// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CoopGame/CoopGame.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef ACVRDrawDebugLine(
	TEXT("COOP.DrawDebugLine"),
	DebugWeaponDrawing, 
	TEXT("Draw line for weapon"), 
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	RootComponent = MeshComp;

	ShootEffectSocketName = "MuzzleFlashSocket";
	TracerTargetName = "BeamEnd";
	BaseDamege = 10.f;
	FireSpeed = 600.f;
	BulletSpreadDegree = 3.f;

	SetReplicates(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenFire = 60 / FireSpeed;
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}

void ASWeapon::Fire()
{
	if (!HasAuthority())
	{
		ServerFire();
	}

	AActor* WeaponOwner = GetOwner();
	if (WeaponOwner)
	{
		FHitResult Hit;
		FVector EyeLocation;
		FRotator EyeRotation;
		FVector MuzzleLocation = MeshComp->GetSocketLocation(ShootEffectSocketName);

		WeaponOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		float HalfRad = FMath::DegreesToRadians(BulletSpreadDegree);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector EndLocation = EyeLocation + ShotDirection * 10000.f;
		FVector TraceEndPoint = EndLocation;

		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(WeaponOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;
		EPhysicalSurface SurfaceType = SurfaceType_Default;
		if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, EndLocation, COLLISION_CHANNEL_WEAPON, QueryParams))
		{
			AActor* HitActor = Hit.GetActor();
			TraceEndPoint = Hit.ImpactPoint;

			float ImpactDamege = BaseDamege;

			
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			
			//FString NewString = FString::FromInt(EPhysicalSurface);
			
			PlayImpactEffect(SurfaceType, TraceEndPoint, MuzzleLocation);
			

			UGameplayStatics::ApplyPointDamage(HitActor, ImpactDamege, ShotDirection, Hit, WeaponOwner->GetInstigatorController(), WeaponOwner, DamageType);

		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, EndLocation, FColor::Red, false, 2.0f, 0, 0.5f);
		}
		

		if (FireShake)
		{
			APawn* WeaponOwnerPawn = Cast<APawn>(WeaponOwner);
			if (WeaponOwnerPawn)
			{
				APlayerController* PC = Cast<APlayerController>(WeaponOwnerPawn->GetController());
				if (PC)
				{
					PC->ClientPlayCameraShake(FireShake);
				}
			}
		}

		if (HasAuthority())
		{
			HitScanTrace.TraceStart = MuzzleLocation;
			HitScanTrace.TraceEnd = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;

		}
		PlayFireEffect(MuzzleLocation, TraceEndPoint);
		
		LastFireSpeed = GetWorld()->GetTimeSeconds();
	}

	
}

void ASWeapon::PlayFireEffect(FVector MuzzleLocation, FVector TraceEndPoint)
{
	if (ShootEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(ShootEffect, RootComponent, ShootEffectSocketName);
	}
	if (TraceEffect)
	{

		UParticleSystemComponent* TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);

		if (TraceComp)
		{
			TraceComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	//UGameplayStatics::SpawnSoundAtLocation(this, FireSound, GetActorLocation());
}

void ASWeapon::PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint, FVector MuzzleLocation)
{
	UParticleSystem* ImpactEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACETYPE_DEFAULT:
		//ImpactDamege = BaseDamege;
		UE_LOG(LogTemp, Warning, TEXT("Base"));
		ImpactEffect = VulnerableImpactEffect;
		break;
	case SURFACETYPE_VULNERABLE:
		ImpactEffect = VulnerableImpactEffect;
		UE_LOG(LogTemp, Warning, TEXT("Head"));
		//ImpactDamege = 3.0f * BaseDamege;
		break;
	default:
		ImpactEffect = DefaultImpactEffect;
		break;
	}
	if (ImpactEffect)
	{
		FVector ImpactNormal = ImpactPoint - MuzzleLocation;
		ImpactNormal.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, ImpactPoint, ImpactNormal.Rotation());
	}
}

void ASWeapon::OnRep_HitScanTrace()
{
	PlayFireEffect(HitScanTrace.TraceStart, HitScanTrace.TraceEnd);
	PlayImpactEffect(HitScanTrace.SurfaceType, HitScanTrace.TraceEnd, HitScanTrace.TraceStart);
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}

void ASWeapon::StartFire()
{
	//float FirstDelay = FMath::Max(LastFireSpeed - GetWorld()->GetTimeSeconds() + TimeBetweenFire, 0.0f);
	GetWorldTimerManager().SetTimer(FireBetweenShots_TimerHandle, this, &ASWeapon::Fire, TimeBetweenFire, true, 0.0f);
}

void ASWeapon::EndFire()
{
	GetWorldTimerManager().ClearTimer(FireBetweenShots_TimerHandle);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}