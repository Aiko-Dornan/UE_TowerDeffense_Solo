//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "WeaponBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AWeaponBase : public AActor
//{
//	GENERATED_BODY()
//
//	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Weapon",meta=(AllowPrivateAccess="true"))
//	class USKeletalMeshComponent* Mesh;
//	
//public:	
//	// Sets default values for this actor's properties
//	AWeaponBase();
//
//	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Gameplay")
//	USoundBase* FireSound;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
//	FVector MuzzleOffset;
//
//	UFUNCTION(BlueprintCallable,Category="Action")
//	void Fire();
//
//	UFUNCTION(BlueprintCallable, Category = "Action")
//	void FireEffect();
//
//	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Action")
//	void FireAction();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
//	bool bCanFire;
//
//	UFUNCTION(BlueprintCallable, Category = "Gameplay")
//	void SetCanFire();
//
//public:	
//	
//
//	FORCEINLINE class USkeletalMeshComponent* GetMesh() const { return Mesh; }
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float MaxAmmo;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float Ammo;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float StockAmmo;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float WeaponDamage;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float FireRate;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float FireRange;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//	float FireSpread;
//
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetMaxAmmo();
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetAmmo();
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetStockAmmo();
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetWeaponDamage();
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetFireRate();
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetFireRange();
//	UFUNCTION(BlueprintPure, Category = "Weapon")
//	float GetFireSpread();
//
//};
