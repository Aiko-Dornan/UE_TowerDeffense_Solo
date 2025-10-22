//// Fill out your copyright notice in the Description page of Project Settings.
//
//

#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"


#include"Kismet/GameplayStatics.h"
#include"Kismet/KismetMathLibrary.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	/*Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));*/
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = Mesh;

	bCanFire = true;

	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	//bIsFullAuto = true;

	MaxAmmo = 30.0f;
	Ammo = 30.0f;
	StockAmmo = 300.0f;
	WeaponDamage = 15.0f;
	FireRate = 0.5f;
	FireRange = 3000.0f;
	FireSpread = 2.5f;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	

}

void AWeaponBase::Fire()
{
	if (bCanFire==true)
	{
		if (GetAmmo()>0.0f)
		{
			FireAtackAction();

			FireEffect();

			Ammo = FMath::Clamp(GetAmmo() - 1.0f, 0.0f, GetMaxAmmo());

			bCanFire = false;

			FTimerManager& timeManager = GetWorldTimerManager();
			FTimerHandle _TimerHandle;
			GetWorldTimerManager().SetTimer(_TimerHandle, this, &AWeaponBase::SetCanFire, AWeaponBase::GetFireRate(), false);

		}


	}



}

void AWeaponBase::FireAtackAction()
{
	if (!ProjectileClass) return;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	/*FVector MuzzleLocation = GetActorLocation() + MuzzleOffset;
	FRotator MuzzleRotation;*/

	UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(GetRootComponent());
	if (!StaticMeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("RootComponent is not a StaticMeshComponent"));
		return;
	}

	FName MuzzleSocketName = "FireSocket"; // �\�P�b�g���� "Muzzle" �̏ꍇ�B�K�v�ɉ����ĕύX�B

	if (!Mesh->DoesSocketExist(MuzzleSocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket %s not found on mesh"), *MuzzleSocketName.ToString());
		return;
	}

	FVector MuzzleLocation = StaticMeshComp->GetSocketLocation(MuzzleSocketName);
	FRotator MuzzleRotation = StaticMeshComp->GetSocketRotation(MuzzleSocketName);

	 //�\�P�b�g�ʒu�E��]�̎擾
	//FVector MuzzleLocation = Mesh->GetSocketLocation(MuzzleSocketName);
	//FRotator MuzzleRotation = Mesh->GetSocketRotation(MuzzleSocketName);
	

	// �J�����̕������擾
	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	//MuzzleRotation = EyeRotation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = OwnerActor->GetInstigator();

	// �e���X�|�[��
	GetWorld()->SpawnActor<AMyProjectileActor>(
		ProjectileClass,
		MuzzleLocation,
		MuzzleRotation,
		SpawnParams
	);
}

void AWeaponBase::SetCanFire()
{
	bCanFire = true;
}

void AWeaponBase::FireEffect()
{

	if (FireSound!=nullptr)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

}

float AWeaponBase::GetMaxAmmo()
{
	return MaxAmmo;
}

float AWeaponBase::GetAmmo()
{
	return Ammo;
}

float AWeaponBase::GetStockAmmo()
{
	return StockAmmo;
}

float AWeaponBase::GetWeaponDamage()
{
	return WeaponDamage;
}

float AWeaponBase::GetFireRate()
{
	return FireRate;
}

float AWeaponBase::GetFireRange()
{
	return FireRange;
}

float AWeaponBase::GetFireSpread()
{
	return FireSpread;
}


