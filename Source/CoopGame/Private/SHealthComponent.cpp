// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SCoopGameMode.h"
// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DefaultHealth = 100.f;
	IsDead = false;
	TeamNumber = 255;
	SetIsReplicatedByDefault(true);
	//SetIsReplicated(true);

}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* ComponentOwner = GetOwner();
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeDamage);

		// ...
	}
	Health = DefaultHealth;
	
}


void USHealthComponent::OnRep_HealthChanged(float OldHealth)
{
	OnHealthChanged.Broadcast(this, Health, OldHealth - Health, nullptr, nullptr, nullptr);
}

// Called every frame
void USHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0 || IsDead)
	{
		return;
	}
	if (Damage > 0)
	{
		Health -= Damage;
		Health = FMath::Clamp(Health, 0.0f, DefaultHealth);
	}
	if (IsFriendly(DamagedActor, DamageCauser) && DamageCauser != DamagedActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Toggled HandleTakeDamage"));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (Health <= 0)
	{
		ASCoopGameMode* GM = Cast<ASCoopGameMode>(GetWorld()->GetAuthGameMode());
		if (GM && DamageCauser && InstigatedBy)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
		IsDead = true;
	}
}


void USHealthComponent::Heal(float HealAmount)
{
	if (Health <= 0.f || HealAmount <= 0.f)
	{
		return;
	}
	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);
	UE_LOG(LogTemp, Log, TEXT("Heal Character %s"), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -1 * HealAmount, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetHealth() const
{
	return Health;
}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}
	USHealthComponent* CompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* CompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	if (CompA == nullptr || CompB == nullptr)
	{
		return true;
	}
	return CompA->TeamNumber == CompB->TeamNumber;
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHealthComponent, Health);
}