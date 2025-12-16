// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include"WeaponBase.h"
#include "AmmoDisplayWidget.h"   // ← これを追加！
#include "GameFramework/Character.h"
#include "InventorySlot.h"
#include"InventorySlotWidget.h"
#include"InventoryWidget.h"
#include "Components/TextRenderComponent.h"
#include "MyHeroPlayer.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AMyHeroPlayer : public ACharacter
{
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyHeroPlayer();

	// 消耗品インベントリ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<FInventorySlot> Inventory;

	UFUNCTION(BlueprintCallable)
	void AddItemToInventory(TSubclassOf<AItemBase> ItemClass, int32 Quantity = 1);

	//// 消耗品用インベントリ（スロット型）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventorySlot> ConsumableInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 InventorySlots = 27; // 3x9など

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	class UInventoryWidget* InventoryWidget;

	// 現在選択しているスロット番号
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SelectedSlotIndex = 0;

	// インベントリスロット数（UIと揃える）
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxInventorySlots = 9; // SlotCount と同じにする

	// エディタから指定可能な武器クラス
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Weapon")
	TSubclassOf<AWeaponBase> GunComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeaponBase> GunComponentSub;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool MainSubFlag{ false };

	// 現在装備中の武器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeaponBase* CurrentWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void BeginZoom();
	void EndZoom();

	

protected:
	
	bool bIsZooming = false;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomedFOV = 45.0f;   // 右クリック時のFOV

	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomInterpSpeed = 20.0f;  // FOV補間スピード

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void MoveForward(float value);

	UFUNCTION()
	void MoveRight(float value);

	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	UFUNCTION()
	void HandleFire();

	UFUNCTION()
	void OnFirePressed();

	UFUNCTION()
	void OnFireReleased();

	UFUNCTION()
	void OnReloadPressed();

	// 武器をスポーン＆装備する関数
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	UFUNCTION()
	void ItemInteract();

	UFUNCTION(BlueprintCallable)
	void PickupItem(class AItemBase* Item);

	// 消耗品追加・使用
	bool AddConsumableToInventory(AItemBase* Item, int32 Quantity = 1);
	bool UseConsumable(int32 SlotIndex);

	void OnMouseWheel(float Value);
	void UseSelectedItem();

	UFUNCTION()//武器ドロップ
	void DropCurrentWeapon();

	//武器切り替え
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchWeapon();

	UPROPERTY(VisibleAnywhere)
	class AAmmoBox* ammobox;

	UFUNCTION()
	void CheatGunEquip(TSubclassOf<AWeaponBase> NewWeaponClass);

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	
	TSubclassOf<class UUserWidget> AmmoWidgetClass;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UAmmoDisplay> AmmoWidgetClass;  // ← ここを UUserWidget から変更

	class UAmmoDisplay* AmmoWidget;
	//Getter

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentStockAmmo() const;

	UFUNCTION()
	void OnAmmoChanged(int32 CurrentAmmo, int32 StockAmmo);

	UFUNCTION()
	void OnReloadStateChanged(bool bIsReloading);

	// プレイヤーのHP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float PlayerHP = 100.0f;

	// ダメージ処理
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void VaultAmmoNum();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ammo_stock_main = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ammo_magazine_main = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ammo_stock_sub = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ammo_magazine_sub = 0;

	float GetMaxHP() const;
	float GetCurrentHP() const;

	private:

		/*UPROPERTY(VisibleAnywhere)
		class UTextRenderComponent* DebugInventoryText;

		void ShowInventoryDebug();*/


		UPROPERTY(VisibleAnywhere, Category = "Weapon")
		TSubclassOf<AWeaponBase> GunComponentVault;

		int MaxHP = 100;
		
};
