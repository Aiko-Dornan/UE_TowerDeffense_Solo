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
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "MyHeroPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlayerAnimType : uint8
{
	Idle       /* UMETA(DisplayName = "Idle")*/,
	Move_Front,
	Move_Back,
	Move_Left,
	Move_Right,
	Attack      /*UMETA(DisplayName = "Attack")*/,
	RangeAttack /*UMETA(DisplayName = "RangeAttack")*/,
	Dead        /*UMETA(DisplayName = "Dead")*/,
	Damage,
};


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

	enum EPlayerState :uint8
	{
		Idle,
		Move,
		Attack,
		RangeATtack,
		Dead,
		Damage,

	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* MoveFrontAnim;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* MoveBackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* MoveLeftAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* MoveRightAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* AttackAnim;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* RangeAttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* DeadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* DamageAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	EPlayerAnimType PlayerAnimeState = EPlayerAnimType::Idle;

	EPlayerAnimType GetMoveAnimByDirection() const;

	void PlayAnimation(EPlayerAnimType NewType, bool bLoop);

	UAnimationAsset* GetAnimByType(EPlayerAnimType Type) const;

	// 再生中ロック
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAnimationLocked = false;

	// 現在再生中のアニメ種別
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	EPlayerAnimType CurrentAnimType;

	// ロック対象のアニメか？
	bool IsLockedAnim(EPlayerAnimType Type) const;

	void LockRelease();
	FTimerHandle LockReleaseHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void BeginZoom();
	void EndZoom();

	// ===== グレネード軌道表示 =====
	UPROPERTY(EditAnywhere, Category = "Grenade|Aim")
	float GrenadeThrowSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Grenade|Aim")
	float TrajectorySimTime = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Grenade|Aim")
	float TrajectoryFrequency = 30.f;

	UPROPERTY(EditAnywhere, Category = "Grenade|Aim")
	bool bShowGrenadeTrajectory = false;

	void DrawGrenadeTrajectory();

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

	void ThrowGrenade(AItemBase* ItemCDO);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

	// ダメージ処理
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	//死亡処理
	UFUNCTION()
	void EnterSpectatorMode();

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

	void Dash();
	void DashStart();
	void DashEnd();

	private:

		/*UPROPERTY(VisibleAnywhere)
		class UTextRenderComponent* DebugInventoryText;

		void ShowInventoryDebug();*/


		UPROPERTY(VisibleAnywhere, Category = "Weapon")
		TSubclassOf<AWeaponBase> GunComponentVault;

		int MaxHP = 100;
		
		int MaxStamina = 20000;
		int NowStamina = 20000;

		bool DashFlag=false;
		float MoveSpeed = 1.0f;
		bool move_flag = false;
		FTimerHandle AnimLockTimerHandle;
};
