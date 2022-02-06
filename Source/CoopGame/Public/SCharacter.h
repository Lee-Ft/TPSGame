// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;
UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();
	
	void EndZoom();

	void UpdateCameraFOV(float DeltaTime);

	bool IsWantToZoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (ClampMin = 0.1f, ClampMax = 100.0))
		float ZoomSpeed;

	float UnZoomedFOV;
	float ZoomedFOV;

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASWeapon> InitialWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;
	//void LookUp(float Value);

	//UPROPERTY(EditDefaultsOnly, Category = "Health")
		

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
		UCameraComponent* CameraComp;
	UPROPERTY(EditDefaultsOnly, Category = "Component")
		USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
		uint8 TeamNumber;
	USHealthComponent* HealthComp;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
		bool IsCharacterAlive;

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Player")
	void EndFire();
};
