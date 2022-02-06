// Fill out your copyright notice in the Description page of Project Settings.


//#include "AIExplodeTracker.h"
#include "AI/AIExplodeTracker.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"
#include "Scharacter.h"
#include "Components/AudioComponent.h"


// Sets default values
AAIExplodeTracker::AAIExplodeTracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCanEverAffectNavigation(false);

	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent = StaticMeshComp;

	ExplosionDetectionSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionDetectionComp"));
	
	ExplosionDetectionSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionDetectionSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	ExplosionDetectionSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ExplosionDetectionSphereComp->SetupAttachment(RootComponent);

	IsVelChanged = true;

	DistanceTolarence = 100.f;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	IsTrackerExploded = false;
	IsNearTimerTriggerd = false;

}


void AAIExplodeTracker::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NextPointLocation = GetNextNaviPoint();
	}
	
	//HealthComp->OnHealthChanged.AddDynam
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &AAIExplodeTracker::OnTrackerHealthChanged);	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tracker: Health Comp Is Null"));
	}
	if (ExplosionDetectionSphereComp)
	{
		ExplosionDetectionSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAIExplodeTracker::OnTrackerNearCharacter);
	}
	
}

void AAIExplodeTracker::Tick(float DeltaSeconds)
{
	if (HasAuthority())
	{
		if (!IsTrackerExploded)
		{
			float DistacneToTarget = (GetActorLocation() - NextPointLocation).Size();

			if (DistacneToTarget > DistanceTolarence)
			{
				ForceDirection = NextPointLocation - GetActorLocation();
				ForceDirection.Normalize();
				ForceDirection *= 500.f;
				StaticMeshComp->AddForce(ForceDirection, NAME_None, IsVelChanged);

			}
			else
			{
				NextPointLocation = GetNextNaviPoint();
			}
		}
	}
}

FVector AAIExplodeTracker::GetNextNaviPoint()
{
	//ACharacter* TargetCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	AActor* TargetCharacter = nullptr;
	float NearestDis = FLT_MAX;
	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; it++)
	{
		APawn* TestPawn = it->Get();

		if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
		{
			continue;
		}
		float Dis = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
		USHealthComponent* TestHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (TestHealthComp && TestHealthComp->GetHealth() > 0)
		{
			if (Dis < NearestDis)
			{
				NearestDis = Dis;
				TargetCharacter = TestPawn;
			}
		}
	}
	if (TargetCharacter)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), TargetCharacter);
		GetWorldTimerManager().ClearTimer(TimerHandle_RestartSearchPoint);
		GetWorldTimerManager().SetTimer(TimerHandle_RestartSearchPoint, this, &AAIExplodeTracker::RefreshTargetPoint, 5.f, true);

		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
	}

	return GetActorLocation();
}

void AAIExplodeTracker::OnTrackerHealthChanged(USHealthComponent* HealthComponent, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	
	if (!IsTrackerExploded && Health <= 0)
	{
		Explode();
	}

	if (MeshMaterialInst == nullptr)
	{
		MeshMaterialInst = StaticMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, StaticMeshComp->GetMaterial(0));
		//MeshMaterialInst->
	}
	if (MeshMaterialInst)
	{
		MeshMaterialInst->SetScalarParameterValue("LastTakenDamageTime", GetWorld()->GetTimeSeconds());
	}
	//
}

void AAIExplodeTracker::OnTrackerNearCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerCharacter = Cast<ASCharacter>(OtherActor);
	if (!USHealthComponent::IsFriendly(OtherActor, this) && PlayerCharacter && !IsNearTimerTriggerd && !IsTrackerExploded)
	{
		GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this, &AAIExplodeTracker::SelfDamage, 0.5f, true);
		IsNearTimerTriggerd = true;
		UGameplayStatics::SpawnSoundAttached(ExplodeWarningSound, RootComponent);
	}

	
}

void AAIExplodeTracker::RefreshTargetPoint()
{
	NextPointLocation = GetNextNaviPoint();
}

void AAIExplodeTracker::Explode()
{
	if (IsTrackerExploded)
	{
		return;
	}
	StaticMeshComp->SetVisibility(false, true);
	//StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayExplodeEffect();
	IsTrackerExploded = true;
	UGameplayStatics::SpawnSoundAttached(ExplodeSound, RootComponent);

	if (HasAuthority())
	{
		TArray<AActor*> IgnoreActor;
		IgnoreActor.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, 20.f, GetActorLocation(), 200.f, nullptr, IgnoreActor, nullptr, GetInstigatorController(), true);
		UE_LOG(LogTemp, Warning, TEXT("Exploded!"));

		SetLifeSpan(2.f);
	}

}

void AAIExplodeTracker::SelfDamage()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
	
}

void AAIExplodeTracker::PlayExplodeEffect()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeEffect, GetActorLocation());
}
