// Fill out your copyright notice in the Description page of Project Settings.


#include "SPickUpActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SPowerUpActor.h"
#include "SCharacter.h"
// Sets default values
ASPickUpActor::ASPickUpActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.f);
	RootComponent = SphereComp;
	
	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(64.f, 75.f, 75.f);
	DecalComp->SetupAttachment(RootComponent);

	ReSpawnActorDuration = 10.f;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickUpActor::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		SpawnPowerUpActor();
	}
	
}

void ASPickUpActor::SpawnPowerUpActor()
{
	if (PowerUpClass == nullptr)
	{
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpActorInstance = GetWorld()->SpawnActor<ASPowerUpActor>(PowerUpClass, GetTransform(), SpawnParams);
}

// Called every frame
void ASPickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASPickUpActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Judge whether the power up actors should be activated
	ASCharacter* PickedUpCharacter = Cast<ASCharacter>(OtherActor);
	if (PickedUpCharacter)
	{

		Super::NotifyActorBeginOverlap(OtherActor);

		if (PowerUpActorInstance)
		{
			PowerUpActorInstance->ActorBeenActivated(OtherActor);
			PowerUpActorInstance = nullptr;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnActor, this, &ASPickUpActor::SpawnPowerUpActor, ReSpawnActorDuration, false);
	}
}

