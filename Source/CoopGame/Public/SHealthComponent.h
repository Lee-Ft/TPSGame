// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent*, HealthComp, float, Health, float, DeltaHealth, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOPGame), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();
	
	UPROPERTY(EditDefaultsOnly, Category = "Team")
		uint8 TeamNumber;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_HealthChanged, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
		float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
		float DefaultHealth;

	UFUNCTION()
		void OnRep_HealthChanged(float OldHealth);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
		
	UPROPERTY(BlueprintAssignable, Category = "Event")
		FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "Health")
		void Heal(float HealAmount);

	bool IsDead;
	
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	static bool IsFriendly(AActor* ActorA, AActor* AActorB);
};
