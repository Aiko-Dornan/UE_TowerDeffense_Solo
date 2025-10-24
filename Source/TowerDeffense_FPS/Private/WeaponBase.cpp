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
	bIsFiring = false;
	bIsReloading = false;

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
	
	Mesh = Cast<UStaticMeshComponent>(GetRootComponent());

}

void AWeaponBase::Fire()
{
	//if (bCanFire==true)
	//{
	//	if (GetAmmo()>0.0f)
	//	{
	//		FireAtackAction();

	//		FireEffect();

	//		Ammo = FMath::Clamp(GetAmmo() - 1.0f, 0.0f, GetMaxAmmo());

	//		

	//		if (bIsFullAuto && bIsFiring)
	//		{
	//			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::Fire, FireRate, false);
	//		}
	//		else
	//		{
	//			// �P���ˌ����͒ʏ�̔��ˊԊu�Ń��Z�b�g
	//			GetWorldTimerManager().SetTimer(FireResetTimerHandle, this, &AWeaponBase::SetCanFire, FireRate, false);
	//		}

	//	}


	//}

	if (!bCanFire || Ammo <= 0.0f || bIsReloading) return;

	FireAtackAction();
	FireEffect();

	Ammo = FMath::Clamp(Ammo - 1.0f, 0.0f, MaxAmmo);
	bCanFire = false;

	GetWorldTimerManager().SetTimer(FireResetTimerHandle, this, &AWeaponBase::SetCanFire, FireRate, false);

	// �e�؂ꎩ�������[�h
	//if (Ammo <= 0.0f /*&& StockAmmo > 0*/)
	//{
	//	StartReload();
	//}
	//  �e���X�V�C�x���g�𔭉�
	OnAmmoChanged.Broadcast((int32)Ammo, (int32)StockAmmo);
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

	if (!StaticMeshComp->DoesSocketExist(MuzzleSocketName))
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

// �ǉ��F�{�^���������ɌĂ΂��
void AWeaponBase::StartFire()
{
//	if (bIsFiring)
//		return;
//
//	bIsFiring = true;
//	Fire(); // ����1������

	if (bIsFiring || bIsReloading) return;
	bIsFiring = true;

	Fire();

	if (bIsFullAuto)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::Fire, FireRate, true);
	}

}

// �ǉ��F�{�^����������Ă΂��
void AWeaponBase::StopFire()
{
	//bIsFiring = false;

	//// �^�C�}�[���~�߂�
	//GetWorldTimerManager().ClearTimer(FireTimerHandle);
	//GetWorldTimerManager().ClearTimer(FireResetTimerHandle);

	//// �Ăь��Ă�悤�Ƀ��Z�b�g
	//bCanFire = true;

	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().ClearTimer(FireResetTimerHandle);
	bCanFire = true;

}

// ==========================
// �����[�h�֘A
// ==========================
void AWeaponBase::StartReload()
{
	if (bIsReloading || Ammo >= MaxAmmo || StockAmmo <= 0)
		return;

	bIsReloading = true;
	bCanFire = false;

	// �����[�h��
	/*if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D(this, ReloadSound);
	}*/

	OnReloadStateChanged.Broadcast(true);  // �����[�h�J�n��ʒm

	// ReloadTime �b��Ɋ���
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::FinishReload, ReloadTime, false);

	UE_LOG(LogTemp, Log, TEXT("Reload started..."));
}

void AWeaponBase::FinishReload()
{
	if (!bIsReloading) return;

	const float NeededAmmo = MaxAmmo - Ammo;
	const float AmmoToLoad = FMath::Min(NeededAmmo, StockAmmo);

	Ammo += AmmoToLoad;
	StockAmmo -= AmmoToLoad;

	bIsReloading = false;
	bCanFire = true;

	UE_LOG(LogTemp, Log, TEXT("Reload finished: Ammo=%f / Stock=%f"), Ammo, StockAmmo);

	OnReloadStateChanged.Broadcast(false); //  �����[�h�I����ʒm
	OnAmmoChanged.Broadcast((int32)Ammo, (int32)StockAmmo);
}
