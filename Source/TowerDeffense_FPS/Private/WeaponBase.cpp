//// Fill out your copyright notice in the Description page of Project Settings.
//
//

#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include"MyHeroPlayer.h"

#include"Kismet/GameplayStatics.h"
#include"Kismet/KismetMathLibrary.h"

#include "Sound/SoundAttenuation.h"     // USoundAttenuation, FAttenuationSettings
#include "Sound/SoundBase.h"            // USoundBase

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

	UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(GetRootComponent());
	if (!StaticMeshComp) return;

	FName MuzzleSocketName = "FireSocket";
	const FVector MuzzleLocation = StaticMeshComp->GetSocketLocation(MuzzleSocketName);
	const FRotator MuzzleRotation = StaticMeshComp->GetSocketRotation(MuzzleSocketName);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = OwnerActor->GetInstigator();

	
	TArray<AMyGrenadeProjectileActor*> SpawnedProjectilesG;
	TArray<AMyProjectileActor*> SpawnedProjectiles;

	// ======== 通常弾 or 散弾の分岐 =========
	const int32 ShotCount = bIsShotgun ? PelletCount : 1;

	for (int32 i = 0; i < ShotCount; i++)
	{
		FRotator ShootRot = MuzzleRotation;

		if (bIsShotgun)
		{
			// 拡散角度ランダム化
			ShootRot.Yaw += FMath::FRandRange(-PelletSpread, PelletSpread);
			ShootRot.Pitch += FMath::FRandRange(-PelletSpread, PelletSpread);

		}

		if (bIsAmmoGrenade)
		{
			AMyGrenadeProjectileActor* Projectile =
				GetWorld()->SpawnActor<AMyGrenadeProjectileActor>(
					ProjectileGrenadeClass,
					MuzzleLocation,
					ShootRot,
					SpawnParams
				);

			if (Projectile)
			{
				// ★ 銃口の向きに10000cm先をターゲットにする
				FVector ShootDir = ShootRot.Vector();
				Projectile->TargetLocation = MuzzleLocation + ShootDir * 10000.f;
				Projectile->Damage = WeaponDamage;
				SpawnedProjectilesG.Add(Projectile);
				Projectile->CalculateLaunchVelocity(); // ここで velocity を計算
				// 弾同士の衝突を無視
				for (AMyGrenadeProjectileActor* OtherProj : SpawnedProjectilesG)
				{
					if (OtherProj != Projectile)
					{
						Projectile->CollisionComp->IgnoreActorWhenMoving(OtherProj, true);
						OtherProj->CollisionComp->IgnoreActorWhenMoving(Projectile, true);
					}
				}
			}
		}
		else
		{
			AMyProjectileActor* Projectile =
				GetWorld()->SpawnActor<AMyProjectileActor>(
					ProjectileClass,
					MuzzleLocation,
					ShootRot,
					SpawnParams
				);

			if (Projectile)
			{
				Projectile->Damage = WeaponDamage;
				SpawnedProjectiles.Add(Projectile);

				// 弾同士の衝突を無視
				for (AMyProjectileActor* OtherProj : SpawnedProjectiles)
				{
					if (OtherProj != Projectile)
					{
						Projectile->CollisionComp->IgnoreActorWhenMoving(OtherProj, true);
						OtherProj->CollisionComp->IgnoreActorWhenMoving(Projectile, true);
					}
				}
			}
		}

		

		

		

		//UE_LOG(LogTemp, Warning, TEXT("fire by%s,%d,%s"),*GetName(), i, *Projectile->GetName());
	}
}


//void AWeaponBase::FireAtackAction()
//{
//	if (!ProjectileClass) return;
//
//	AActor* OwnerActor = GetOwner();
//	if (!OwnerActor) return;
//
//	UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(GetRootComponent());
//	if (!StaticMeshComp) return;
//
//	FName MuzzleSocketName = "FireSocket";
//	FVector MuzzleLocation = StaticMeshComp->GetSocketLocation(MuzzleSocketName);
//	FRotator MuzzleRotation = StaticMeshComp->GetSocketRotation(MuzzleSocketName);
//
//	FActorSpawnParameters SpawnParams;
//	SpawnParams.Owner = this;
//	SpawnParams.Instigator = OwnerActor->GetInstigator();
//
//	AMyProjectileActor* Projectile = GetWorld()->SpawnActor<AMyProjectileActor>(
//		ProjectileClass,
//		MuzzleLocation,
//		MuzzleRotation,
//		SpawnParams
//	);
//
//	if (Projectile)
//	{
//		Projectile->Damage = WeaponDamage; // 武器の攻撃力を弾に反映
//	}
//}

void AWeaponBase::SetCanFire()
{
	bCanFire = true;
}

void AWeaponBase::FireEffect()
{

	if (FireSound!=nullptr)
	{
		// 銃の位置を取得
		FVector MuzzleLocation = Mesh->GetSocketLocation("FireSocket");

		// Attenuationを指定して3D音を再生
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			MuzzleLocation,
			1.0f,           // 音量
			1.0f,           // ピッチ
			0.0f,           // 開始時間
			FireSoundAttenuation // Attenuationを指定
		);
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

