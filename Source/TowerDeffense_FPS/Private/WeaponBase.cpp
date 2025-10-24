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
	//			// 単発射撃時は通常の発射間隔でリセット
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

	// 弾切れ自動リロード
	//if (Ammo <= 0.0f /*&& StockAmmo > 0*/)
	//{
	//	StartReload();
	//}
	//  弾数更新イベントを発火
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

	FName MuzzleSocketName = "FireSocket"; // ソケット名が "Muzzle" の場合。必要に応じて変更。

	if (!StaticMeshComp->DoesSocketExist(MuzzleSocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket %s not found on mesh"), *MuzzleSocketName.ToString());
		return;
	}

	FVector MuzzleLocation = StaticMeshComp->GetSocketLocation(MuzzleSocketName);
	FRotator MuzzleRotation = StaticMeshComp->GetSocketRotation(MuzzleSocketName);

	 //ソケット位置・回転の取得
	//FVector MuzzleLocation = Mesh->GetSocketLocation(MuzzleSocketName);
	//FRotator MuzzleRotation = Mesh->GetSocketRotation(MuzzleSocketName);
	

	// カメラの方向を取得
	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerActor->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	//MuzzleRotation = EyeRotation;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = OwnerActor->GetInstigator();

	// 弾をスポーン
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

// 追加：ボタン押下時に呼ばれる
void AWeaponBase::StartFire()
{
//	if (bIsFiring)
//		return;
//
//	bIsFiring = true;
//	Fire(); // すぐ1発撃つ

	if (bIsFiring || bIsReloading) return;
	bIsFiring = true;

	Fire();

	if (bIsFullAuto)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::Fire, FireRate, true);
	}

}

// 追加：ボタン離したら呼ばれる
void AWeaponBase::StopFire()
{
	//bIsFiring = false;

	//// タイマーを止める
	//GetWorldTimerManager().ClearTimer(FireTimerHandle);
	//GetWorldTimerManager().ClearTimer(FireResetTimerHandle);

	//// 再び撃てるようにリセット
	//bCanFire = true;

	bIsFiring = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().ClearTimer(FireResetTimerHandle);
	bCanFire = true;

}

// ==========================
// リロード関連
// ==========================
void AWeaponBase::StartReload()
{
	if (bIsReloading || Ammo >= MaxAmmo || StockAmmo <= 0)
		return;

	bIsReloading = true;
	bCanFire = false;

	// リロード音
	/*if (ReloadSound)
	{
		UGameplayStatics::PlaySound2D(this, ReloadSound);
	}*/

	OnReloadStateChanged.Broadcast(true);  // リロード開始を通知

	// ReloadTime 秒後に完了
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

	OnReloadStateChanged.Broadcast(false); //  リロード終了を通知
	OnAmmoChanged.Broadcast((int32)Ammo, (int32)StockAmmo);
}
