//// Fill out your copyright notice in the Description page of Project Settings.
//
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"MyProjectileActor.h"
#include"MyGrenadeProjectileActor.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, StockAmmo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStateChanged, bool, bIsReloading);

// 前方宣言
class USoundBase;
class USoundAttenuation;


UCLASS()
class TOWERDEFFENSE_FPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()

	
//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
//class USkeletalMeshComponent* Mesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	//class UStaticMeshComponent* Mesh;
	
	

//	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	// 弾数変更イベント（Blueprintでも使えるようにBlueprintAssignable）
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnAmmoChanged OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadStateChanged OnReloadStateChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	 UStaticMeshComponent* Mesh;

	 USceneComponent* SceneRoot;

	 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	 bool bIsSubWeapon = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	//class USkeletalMeshComponent* WMesh;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	//USphereComponent* CollisionComp;

	
	/* UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	 UStaticMeshComponent* StaticMeshComp2;*/
	//USkeletalMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Gameplay")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FVector MuzzleOffset;

	UFUNCTION(BlueprintCallable,Category="Action")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Action")
	void FireEffect();

	//UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Action")
	UFUNCTION(BlueprintCallable, Category = "Action")
	void FireAtackAction();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AMyProjectileActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Grenade")
	TSubclassOf<class AMyGrenadeProjectileActor> ProjectileGrenadeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Grenade")
	float ExploveArea = 100.0f;//いらないかも
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bCanFire;
protected:
//Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetCanFire();

	// 銃声用の距離減衰設定
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundAttenuation* FireSoundAttenuation;

	bool bIsFiring = false; // 現在射撃中か

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	FTimerHandle FireTimerHandle;       // フルオート用ループ
	FTimerHandle FireResetTimerHandle;  // 単発用リセット
	FTimerHandle ReloadTimerHandle;

public:	
	/*UFUNCTION(BlueprintPure,Category="Weapon")
	bool IsFullAuto();*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* RangeCollision;



	//FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsFullAuto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float StockAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float WeaponDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireSpread;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Reload")
	float ReloadTime = 2.0f; // リロード時間（秒）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Reload")
	USoundBase* ReloadSound;

	// ========== Shotgun ==========

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun")
	bool bIsShotgun = false;  // ← 散弾ON/OFF

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsAmmoGrenade = false;  // ← GrenadeON/OFF

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun")
	int32 PelletCount = 8;   // ← 発射する弾の数（散弾数）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Shotgun")
	float PelletSpread = 6.0f;  // ← 拡散角度（度）

	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetMaxAmmo();
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetAmmo();
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetStockAmmo();
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetWeaponDamage();
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetFireRate();
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetFireRange();
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetFireSpread();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire(); // 押下開始

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire(); // 押下終了

	// ==== リロード ====
	void StartReload();
	void FinishReload();

	
	private:
		

};
