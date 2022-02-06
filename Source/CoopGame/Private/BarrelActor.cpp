// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrelActor.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ABarrelActor::ABarrelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelComponent"));
	StaticMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent = StaticMeshComp;

	ExplodeForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplodeForceComponent"));
	ExplodeForceComp->bAutoActivate = false;
	ExplodeForceComp->bIgnoreOwningActor = true;
	ExplodeForceComp->bImpulseVelChange = true;
	ExplodeForceComp->SetupAttachment(RootComponent);

	IsExploded = false;

}

// Called when the game starts or when spawned
void ABarrelActor::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ABarrelActor::OnHealthChanged);
	}
	
}

// Called every frame
void ABarrelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABarrelActor::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Barrel was hit!"));
	if (Health <= 0 && !IsExploded)
	{
		Explode();
		IsExploded = true;
	}
}

void ABarrelActor::Explode()
{

	UE_LOG(LogTemp, Warning, TEXT("Barrel was Exploded!"));
	StaticMeshComp->AddImpulse(FVector::UpVector * 500.f, NAME_None, true);
	ExplodeForceComp->FireImpulse();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeEffect, GetActorLocation());


}

