// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SWeapon.h"
#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame/CoopGame.h"
//#include "UnrealMath.h"
// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 800.f;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 10.f;
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomSpeed = 20.0f;
	WeaponSocketName = "equipped_socket";

	TeamNumber = 0;
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_CHANNEL_WEAPON, ECR_Ignore);


	IsCharacterAlive = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanJump = true;

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnWeaponParams;

		SpawnWeaponParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(InitialWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnWeaponParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}
	}

	
	UnZoomedFOV = CameraComp->FieldOfView;
	ZoomedFOV = 65.0f;
	
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
		HealthComp->TeamNumber = TeamNumber;
		UE_LOG(LogTemp, Error, TEXT("Health Comp Is Not Null %d"), *FString::FromInt(HealthComp->TeamNumber));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Health Comp Is Null!!"));
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraFOV(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::EndFire);
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch(true);
}

void ASCharacter::EndCrouch()
{
	UnCrouch(true);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
}

void ASCharacter::BeginZoom()
{
	IsWantToZoom = true;
}

void ASCharacter::EndZoom()
{
	IsWantToZoom = false;
}

void ASCharacter::UpdateCameraFOV(float DeltaTime)
{
	float TargetFOV = IsWantToZoom ? ZoomedFOV : UnZoomedFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}

}

void ASCharacter::EndFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComponent, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && IsCharacterAlive)
	{
		GetMovementComponent()->StopMovementImmediately();
		UE_LOG(LogTemp, Error, TEXT("Dead!"));

		EndFire();

		IsCharacterAlive = false;
		//GetController()->SetIgnoreMoveInput(false);
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		DetachFromControllerPendingDestroy();
		SetLifeSpan(5.f);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetLifeSpan(5.f);
		}


	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, IsCharacterAlive);
}