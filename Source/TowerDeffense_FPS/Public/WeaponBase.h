//// Fill out your copyright notice in the Description page of Project Settings.
//
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"MyProjectileActor.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, StockAmmo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadStateChanged, bool, bIsReloading);

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

	// �e���ύX�C�x���g�iBlueprint�ł��g����悤��BlueprintAssignable�j
	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnAmmoChanged OnAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnReloadStateChanged OnReloadStateChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* Mesh;

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

protected:
//Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bCanFire;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetCanFire();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Weapon")
	bool bIsFullAuto;

	bool bIsFiring = false; // ���ݎˌ�����

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	FTimerHandle FireTimerHandle;       // �t���I�[�g�p���[�v
	FTimerHandle FireResetTimerHandle;  // �P���p���Z�b�g
	FTimerHandle ReloadTimerHandle;

public:	
	/*UFUNCTION(BlueprintPure,Category="Weapon")
	bool IsFullAuto();*/

	//FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }

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
	float ReloadTime = 2.0f; // �����[�h���ԁi�b�j

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Reload")
	USoundBase* ReloadSound;

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
	void StartFire(); // �����J�n

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire(); // �����I��

	// ==== �����[�h ====
	void StartReload();
	void FinishReload();

};
