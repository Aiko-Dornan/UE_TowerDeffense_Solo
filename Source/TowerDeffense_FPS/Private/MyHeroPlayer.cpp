// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHeroPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h" // 忘れずに！
#include"AmmoBox.h"
#include"ItemBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"   // ★ WeaponMesh の参照用
#include "Components/BoxComponent.h"          // ★ DroppedItem->CollisionBox 用
#include"InventorySlotWidget.h"
#include"MySpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include"EnemyCharacterBase.h"
#include"MyGrenadeProjectileActor.h"
#include"TD_GameInstance.h"

// Sets default values
AMyHeroPlayer::AMyHeroPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	check(CameraComponent != nullptr);

	CameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	CameraComponent->bUsePawnControlRotation = true;

	//  カメラの向きに体も追従させる
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	//  移動時に自動で体の向きを変えないように
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//GunComponent = CreateDefaultSubobject<AWeaponBase>(TEXT("/Game/BluePrint/MyWeaponBase"));

	CurrentWeapon = nullptr;

	// コンストラクタで初期化
	//DebugInventoryText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DebugInventoryText"));
	//DebugInventoryText->SetupAttachment(RootComponent);
	//DebugInventoryText->SetRelativeLocation(FVector(0, 0, 200)); // プレイヤー頭上など
	//DebugInventoryText->SetHorizontalAlignment(EHTA_Center);
	//DebugInventoryText->SetWorldSize(30.f);

}

// Called when the game starts or when spawned
void AMyHeroPlayer::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentLevel = GetWorld()->GetMapName();

	if (CurrentLevel.Contains("BaseMap")) // ゲームプレイ画面の場合のみ
	{

		if (CameraComponent)
		{
			DefaultFOV = CameraComponent->FieldOfView;
		}


		// ゲーム開始時に銃を装備
		//if (GunComponent)
		{
			if (UTD_GameInstance* GI = Cast<UTD_GameInstance>(GetGameInstance()))
			{
				GunComponent = GI->ResolveWeapon(GI->SelectedMainWeaponID);
				GunComponentSub = GI->ResolveWeapon(GI->SelectedSubWeaponID);
			}

			EquipWeapon(GunComponent);
		}

		// 装備したメイン武器の弾数を保存
		if (CurrentWeapon)
		{
			ammo_stock_main = CurrentWeapon->StockAmmo;
			ammo_magazine_main = CurrentWeapon->Ammo;
		}

		// サブ武器も初期化（まだ装備しないが弾数だけ保存したい場合）
		if (GunComponentSub)
		{
			// 一時的にスポーンして弾数だけ取って破棄する
			AWeaponBase* TempSub = GetWorld()->SpawnActor<AWeaponBase>(GunComponentSub);
			if (TempSub)
			{
				ammo_stock_sub = TempSub->StockAmmo;
				ammo_magazine_sub = TempSub->Ammo;
				TempSub->Destroy();
			}
		}

		if (AmmoWidgetClass)
		{
			AmmoWidget = CreateWidget<UAmmoDisplay>(GetWorld(), AmmoWidgetClass);
			if (AmmoWidget)
			{
				AmmoWidget->AddToViewport();
				AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
				AmmoWidget->UpdateHP(GetCurrentHP(), GetMaxHP());
			}
		}

		// ------------------ InventoryWidget の生成 ------------------
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();

				// 初期表示用に Inventory を反映
				InventoryWidget->UpdateInventory(Inventory);
			}
		}

		MaxHP = PlayerHP;

	}

	// インベントリ初期化
	//ConsumableInventory.SetNum(InventorySlots);

	/*if (AmmoWidgetClass)
	{
		UUserWidget* AmmoWidget = CreateWidget<UUserWidget>(GetWorld(), AmmoWidgetClass);
		if (AmmoWidget)
		{
			AmmoWidget->AddToViewport();
		}
	}*/

}

// Called every frame
void AMyHeroPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + CameraComponent->GetForwardVector() * 500.0f;

	FVector BoxHalfSize(10, 10, 10);
	FRotator Orientation = CameraComponent->GetComponentRotation();

	DrawDebugBox(
		GetWorld(),
		End,
		BoxHalfSize,
		Orientation.Quaternion(),
		FColor::Green,
		false,
		0.f,
		0,
		1.f
	);

	// -------- Zoom 処理 --------
	float TargetFOV = bIsZooming ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(
		CameraComponent->FieldOfView,
		TargetFOV,
		DeltaTime,
		ZoomInterpSpeed
	);
	CameraComponent->SetFieldOfView(NewFOV);

	if (bShowGrenadeTrajectory)
	{
		DrawGrenadeTrajectory();
	}

	

}

// Called to bind functionality to input
void AMyHeroPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyHeroPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyHeroPlayer::MoveRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AMyHeroPlayer::StartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AMyHeroPlayer::StopJump);
	
	PlayerInputComponent->BindAxis("Turn", this, &AMyHeroPlayer::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up", this, &AMyHeroPlayer::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyHeroPlayer::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMyHeroPlayer::OnFireReleased);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyHeroPlayer::ItemInteract);
	PlayerInputComponent->BindAction("Switch", IE_Pressed, this, &AMyHeroPlayer::SwitchWeapon);

	PlayerInputComponent->BindAxis("MouseWheel", this, &AMyHeroPlayer::OnMouseWheel);
	PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AMyHeroPlayer::UseSelectedItem);

	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyHeroPlayer::HandleFire);
	// クリックで射撃
	
	//reload
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMyHeroPlayer::OnReloadPressed);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMyHeroPlayer::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMyHeroPlayer::EndZoom);



}

void AMyHeroPlayer::HandleFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}

	

}

void AMyHeroPlayer::OnFirePressed()
{
	if (!CurrentWeapon) return;

	if (CurrentWeapon->bIsFullAuto)
	{
		
		// フルオート開始
		CurrentWeapon->StartFire();
		VaultAmmoNum();
	}
	else
	{
		
		// セミオートは一発だけ
		CurrentWeapon->Fire();
		VaultAmmoNum();
	}

	if (AmmoWidget)
	{
		AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
	}
}

void AMyHeroPlayer::OnFireReleased()
{
	if (!CurrentWeapon) return;
	if (CurrentWeapon->bIsFullAuto)
	{
		
		CurrentWeapon->StopFire();
		VaultAmmoNum();
	}

	if (AmmoWidget)
	{
		AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
	}
}

void AMyHeroPlayer::OnReloadPressed()
{
	if (CurrentWeapon)
	{
		
		CurrentWeapon->StartReload();
		VaultAmmoNum();
	}

	if (AmmoWidget)
	{
		AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
	}

}

void AMyHeroPlayer::BeginZoom()
{
	bIsZooming = true;
}

void AMyHeroPlayer::EndZoom()
{
	bIsZooming = false;
}

void AMyHeroPlayer::ItemInteract()
{
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + CameraComponent->GetForwardVector() * 500.0f;

	// 四角形の半径（XYZ方向のサイズ）
	FVector BoxHalfSize(10.f, 10.f, 10.f);

	// 角度（今回は回転なし）
	FRotator Orientation = CameraComponent->GetComponentRotation();

	FHitResult Hit;

	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxHalfSize,
		Orientation,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{ this },
		EDrawDebugTrace::ForDuration,   // ← 四角形が視覚化される！
		Hit,
		true,
		FLinearColor::Green,
		FLinearColor::Red,
		0.1f
	);

	if (bHit)
	{
		// アイテム取得
		if (AItemBase* Item = Cast<AItemBase>(Hit.GetActor()))
		{
			PickupItem(Item);
			return;
		}

		// AmmoBox (既存の処理)
		if (AAmmoBox* AmmoBoxHit = Cast<AAmmoBox>(Hit.GetActor()))
		{
			AmmoBoxHit->TryGiveAmmo(CurrentWeapon);
			VaultAmmoNum();

			if (AmmoWidget)
				AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Nothing to interact with."));
}

bool AMyHeroPlayer::AddConsumableToInventory(AItemBase* Item, int32 Quantity)
{
	if (!Item || Item->ItemType != EItemType::IT_Consumable || Item->ItemType != EItemType::IT_Grenade) return false;

	// 既存の同じアイテムにスタック
	for (FInventorySlot& Slot : ConsumableInventory)
	{
		if (Slot.ItemClass == Item->GetClass())
		{
			Slot.Quantity += Quantity;
			Item->Destroy();
			return true;
		}
	}

	// 空スロットに追加
	for (FInventorySlot& Slot : ConsumableInventory)
	{
		if (Slot.IsEmpty())
		{
			Slot.ItemClass = Item->GetClass();
			Slot.Quantity = Quantity;
			Item->Destroy();
			return true;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Inventory full!"));
	return false;
}

void AMyHeroPlayer::AddItemToInventory(TSubclassOf<AItemBase> ItemClass, int32 Quantity)
{
	if (!ItemClass) return;

	//既存の同じアイテムにスタック
	for (FInventorySlot& Slot : Inventory)
	{
		if (Slot.ItemClass == ItemClass&&Slot.Quantity<3)
		{
			Slot.Quantity += Quantity;
			return;
		}
	}

	// 空スロットに追加（これが大事！！）
	for (FInventorySlot& Slot : Inventory)
	{
		if (Slot.IsEmpty())
		{
			Slot.ItemClass = ItemClass;
			Slot.Quantity = Quantity;
			return;
		}
	}

	// 空が全く無い場合のみ、新枠として追加
	FInventorySlot NewSlot;
	NewSlot.ItemClass = ItemClass;
	NewSlot.Quantity = Quantity;
	Inventory.Add(NewSlot);
}

//void AMyHeroPlayer::ShowInventoryDebug()
//{
//	FString InventoryStr = TEXT("Inventory:\n");
//	for (const FInventorySlot& Slot : Inventory)
//	{
//		if (Slot.IsEmpty())
//			InventoryStr += TEXT("- Empty\n");
//		else
//		{
//			AItemBase* DefaultItem = Slot.ItemClass->GetDefaultObject<AItemBase>();
//			InventoryStr += FString::Printf(TEXT("- %s x%d\n"), *DefaultItem->GetName(), Slot.Quantity);
//		}
//	}
//
//	if (DebugInventoryText)
//		DebugInventoryText->SetText(FText::FromString(InventoryStr));
//}

//bool AMyHeroPlayer::UseConsumable(int32 SlotIndex)
//{
//	if (!ConsumableInventory.IsValidIndex(SlotIndex)) return false;
//
//	FInventorySlot& Slot = ConsumableInventory[SlotIndex];
//	if (Slot.IsEmpty()) return false;
//
//	// 回復処理
//	if (Slot.ItemClass)
//	{
//		AItemBase* TempItem = GetWorld()->SpawnActor<AItemBase>(Slot.ItemClass);
//		if (TempItem && TempItem->HealAmount > 0.f)
//		{
//			// HP回復
//			//PlayerHP = FMath::Clamp(PlayerHP + TempItem->HealAmount, 0.f, MaxHP);
//			if (AmmoWidget)
//				AmmoWidget->UpdateHP(PlayerHP, MaxHP);
//
//			TempItem->Destroy();
//		}
//	}
//
//	// スタック減少
//	Slot.Quantity--;
//	if (Slot.Quantity <= 0)
//		Slot.ItemClass = nullptr;
//
//	return true;
//}

void AMyHeroPlayer::UseSelectedItem()
{
	if (!Inventory.IsValidIndex(SelectedSlotIndex))
		return;

	FInventorySlot& Slot = Inventory[SelectedSlotIndex];

	if (Slot.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot empty"));
		return;
	}

	AItemBase* ItemCDO = Slot.ItemClass->GetDefaultObject<AItemBase>();

	if (ItemCDO->ItemType == EItemType::IT_Consumable)
	{
		// ★ 消耗品を使用
		Slot.Quantity--;

		if (Slot.Quantity <= 0)
			Slot.ItemClass = nullptr;

		// 回復処理
		PlayerHP = FMath::Clamp(PlayerHP + ItemCDO->HealAmount, 0.f, MaxHP);

		if (AmmoWidget)
			AmmoWidget->UpdateHP(PlayerHP, MaxHP);

		UE_LOG(LogTemp, Warning, TEXT("Used item: %s"), *ItemCDO->GetName());

		// UI更新
		if (InventoryWidget)
			InventoryWidget->UpdateInventory(Inventory);
	}
	if (ItemCDO->ItemType == EItemType::IT_Grenade)
	{
		// ★ 消耗品を使用
		Slot.Quantity--;

		if (Slot.Quantity <= 0)
			Slot.ItemClass = nullptr;

		//ここでグレネードを投げる(スポーンさせる)処理がしたい
		ThrowGrenade(ItemCDO);

		// UI更新
		if (InventoryWidget)
			InventoryWidget->UpdateInventory(Inventory);
	}

}

void AMyHeroPlayer::OnMouseWheel(float Value)
{
	if (Value > 0)
	{
		SelectedSlotIndex = (SelectedSlotIndex + 1) % MaxInventorySlots;
	}
	else if (Value < 0)
	{
		SelectedSlotIndex = (SelectedSlotIndex - 1 + MaxInventorySlots) % MaxInventorySlots;
	}

	UE_LOG(LogTemp, Warning, TEXT("Selected Slot = %d"), SelectedSlotIndex);

	
	// ===== グレネードかどうか判定 =====
	bShowGrenadeTrajectory = false;

	if (Inventory.IsValidIndex(SelectedSlotIndex))
	{
		const FInventorySlot& Slot = Inventory[SelectedSlotIndex];
		if (!Slot.IsEmpty())
		{
			AItemBase* ItemCDO = Slot.ItemClass->GetDefaultObject<AItemBase>();
			if (ItemCDO && ItemCDO->ItemType == EItemType::IT_Grenade)
			{
				bShowGrenadeTrajectory = true;
			}
		}
	}

	// UI側に知らせる
	if (InventoryWidget)
		InventoryWidget->HighlightSlot(SelectedSlotIndex);
}

void AMyHeroPlayer::PickupItem(AItemBase* Item)
{
	if (!Item) return;

	// 武器は従来通り
	if (Item->ItemType == EItemType::IT_Weapon)
	{
		DropCurrentWeapon();
		Item->OnPickedUp(this);

		if (AmmoWidget)
		{
			AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
		}
	}

	// 消耗品（インベントリへ入れる）
	else if (Item->ItemType == EItemType::IT_Consumable|| Item->ItemType == EItemType::IT_Grenade)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateInventory called!"));
		AddItemToInventory(Item->GetClass(), 1);

		// UI更新
		if (InventoryWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateInventory called2!"));
			InventoryWidget->UpdateInventory(Inventory);
		}
	}

	// デバッグ表示
	//ShowInventoryDebug();

	Item->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Picked up item: %s"), *Item->GetName());
}

void AMyHeroPlayer::ThrowGrenade(AItemBase* ItemCDO)
{
	if (!ItemCDO || !ItemCDO->GrenadeActorClass) return;

	FVector SpawnLocation =
		CameraComponent->GetComponentLocation()
		+ CameraComponent->GetForwardVector() * 100.f;

	FRotator SpawnRotation = CameraComponent->GetComponentRotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	AMyGrenadeProjectileActor* Grenade = GetWorld()->SpawnActor<AMyGrenadeProjectileActor>(
		ItemCDO->GrenadeActorClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);

	if (Grenade)
	{
		Grenade->Damage;
		Grenade->AmountAreaAtacck;
		FVector ShootDir = SpawnRotation.Vector();
		Grenade->TargetLocation = SpawnLocation + ShootDir * 20000.f;
		
		
	
		Grenade->CalculateLaunchVelocity(); // ここで velocity を計算
		/*UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Grenade->GetRootComponent());
		if (Prim)
		{
			Prim->AddImpulse(
				CameraComponent->GetForwardVector() * 1200.f,
				NAME_None,
				true
			);
		}*/
	}
}

//void AMyHeroPlayer::PickupItem(AItemBase* Item)
//{
//	if (!Item) return;
//
//	// ▼ ここで古い武器をドロップする
//	if (Item->bIsWeapon)
//	{
//		DropCurrentWeapon();
//	}
//
//	// アイテム側の処理
//	Item->OnPickedUp(this);
//
//	// UI更新
//	if (AmmoWidget)
//	{
//		AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
//	}
//}


void AMyHeroPlayer::DropCurrentWeapon()
{
	if (!CurrentWeapon) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 300.0f + FVector(0, 30, 50);
	FRotator DropRotation = FRotator(0, 0, 0);//GetActorRotation();

	// ▼ AItemBase をスポーン（変換ではない）
	AItemBase* DroppedItem = GetWorld()->SpawnActor<AItemBase>(
		AItemBase::StaticClass(),
		DropLocation,
		DropRotation,
		Params
	);

	if (DroppedItem)
	{
		DroppedItem->bIsWeapon = true;

		// ▼ 武器クラスをコピー
		DroppedItem->WeaponClass = CurrentWeapon->GetClass();

		// ▼ 弾数コピー
		DroppedItem->SavedAmmo = CurrentWeapon->Ammo;
		DroppedItem->SavedStockAmmo = CurrentWeapon->StockAmmo;
		DroppedItem->ItemType = EItemType::IT_Weapon;
		// ▼ メッシュの見た目を WeaponBase と合わせる（必要なら）
		if (CurrentWeapon->GetMesh())
		{
			DroppedItem->Mesh->SetStaticMesh(CurrentWeapon->GetMesh()->GetStaticMesh());
		}

		DroppedItem->Mesh->SetSimulatePhysics(true);
		DroppedItem->Mesh->AddImpulse(GetActorForwardVector() * 200);

		/*DroppedItem->Mesh->BodyInstance.bLockXRotation = true;
		DroppedItem->Mesh->BodyInstance.bLockYRotation = true;
		DroppedItem->Mesh->BodyInstance.bLockZRotation = true;*/

	}

	// ▼ 元の武器を破棄
	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;
}


void AMyHeroPlayer::CheatGunEquip(TSubclassOf<AWeaponBase> NewWeaponClass)
{
	


}

void AMyHeroPlayer::MoveForward(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, value);
}

void AMyHeroPlayer::MoveRight(float value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void AMyHeroPlayer::StartJump()
{
	bPressedJump = true;
}

void AMyHeroPlayer::StopJump()
{
	bPressedJump = false;
}

float AMyHeroPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//PlayerHP -= ActualDamage;
	PlayerHP = FMath::Clamp(PlayerHP - ActualDamage, 0.0f, MaxHP);


	UE_LOG(LogTemp, Warning, TEXT("Player took %f damage! HP = %f"), ActualDamage, PlayerHP);

	if (AmmoWidget)
	{
		AmmoWidget->UpdateHP(PlayerHP, MaxHP);
	}

	if (PlayerHP <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is dead!"));
		// TODO: 死亡処理（リスポーン・ゲームオーバーUIなど）
		
		EnterSpectatorMode();
	}

	return ActualDamage;
}

void AMyHeroPlayer::EnterSpectatorMode()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	bIsDead = true;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacterBase::StaticClass(), Enemies);

	for (AActor* Enemy : Enemies)
	{
		if (AEnemyCharacterBase* E = Cast<AEnemyCharacterBase>(Enemy))
		{
			if (E->CurrentTarget == this)
			{
				E->ChooseTargetBP();
			}
		}
	}

	// ===== プレイヤーキャラを無効化 =====
	DisableInput(PC);
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);

	// 武器を非表示（Destroy しないのがポイント）
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorEnableCollision(false);
	}

	// ===== SpectatorPawn をスポーン =====
	FActorSpawnParameters Params;
	Params.Owner = PC;

	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(
		ASpectatorPawn::StaticClass(),
		GetActorLocation(),
		GetActorRotation(),
		Params
	);

	if (Spectator)
	{
		PC->Possess(Spectator);
	}




}

void AMyHeroPlayer::VaultAmmoNum()
{
	if (!MainSubFlag)
	{
		ammo_stock_main = CurrentWeapon->StockAmmo;
		ammo_magazine_main = CurrentWeapon->Ammo;
	}
	else
	{
		ammo_stock_sub = CurrentWeapon->StockAmmo;
		ammo_magazine_sub = CurrentWeapon->Ammo;
	}
}

void AMyHeroPlayer::SwitchWeapon()
{
	if (MainSubFlag)
	{
		GunComponentVault = GunComponent;
		MainSubFlag = false;
	}
	else
	{
		GunComponentVault = GunComponentSub;
		MainSubFlag = true;
	}

	EquipWeapon(GunComponentVault);

	if (!MainSubFlag)
	{
		CurrentWeapon->StockAmmo = ammo_stock_main;
		CurrentWeapon->Ammo = ammo_magazine_main;
	}
	else
	{
		CurrentWeapon->StockAmmo = ammo_stock_sub;
		CurrentWeapon->Ammo = ammo_magazine_sub;
		
	}

	if (AmmoWidget)
	{
		AmmoWidget->UpdateAmmoText(GetCurrentAmmo(), GetCurrentStockAmmo());
	}
	
}

void AMyHeroPlayer::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!WeaponClass) return;

	// 既存の武器を削除
	if (CurrentWeapon)
	{
		


		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	// 武器をスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);

	if (SpawnedWeapon)
	{
		const FName WeaponSocketName = TEXT("WeaponSocket"); // スケルトンメッシュのソケット名に一致させる
		SpawnedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		CurrentWeapon = SpawnedWeapon;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->OnAmmoChanged.AddDynamic(this, &AMyHeroPlayer::OnAmmoChanged);
		CurrentWeapon->OnReloadStateChanged.AddDynamic(this, &AMyHeroPlayer::OnReloadStateChanged);
	}

	
}

int32 AMyHeroPlayer::GetCurrentAmmo() const
{
	if (CurrentWeapon)
	{
		return (int32)CurrentWeapon->GetAmmo();
	}
	return 0;
}

int32 AMyHeroPlayer::GetCurrentStockAmmo() const
{
	if (CurrentWeapon)
	{
		return (int32)CurrentWeapon->GetStockAmmo();
	}
	return 0;
}

void AMyHeroPlayer::OnAmmoChanged(int32 CurrentAmmo, int32 StockAmmo)
{
	if (AmmoWidget)
	{
		AmmoWidget->UpdateAmmoText(CurrentAmmo, StockAmmo);
	}
}

void AMyHeroPlayer::OnReloadStateChanged(bool bIsReloading)
{
	if (AmmoWidget)
	{
		AmmoWidget->SetReloading(bIsReloading);
		VaultAmmoNum();
	}
}

float AMyHeroPlayer::GetCurrentHP() const
{
	return (float)PlayerHP;
}

float AMyHeroPlayer::GetMaxHP() const
{
	return (float)MaxHP;
}

void AMyHeroPlayer::DrawGrenadeTrajectory()
{
	FVector StartLocation =
		CameraComponent->GetComponentLocation()
		+ CameraComponent->GetForwardVector() * 50.f;

	FVector LaunchVelocity =
		CameraComponent->GetForwardVector() * GrenadeThrowSpeed;

	FPredictProjectilePathParams Params;
	Params.StartLocation = StartLocation;
	Params.LaunchVelocity = LaunchVelocity;
	Params.bTraceWithCollision = true;
	Params.ProjectileRadius = 5.f;
	Params.MaxSimTime = TrajectorySimTime;
	Params.SimFrequency = TrajectoryFrequency;
	Params.OverrideGravityZ = 0.f; // 0 = WorldGravity
	Params.TraceChannel = ECC_Visibility;
	Params.ActorsToIgnore.Add(this);
	Params.DrawDebugType = EDrawDebugTrace::None;

	FPredictProjectilePathResult Result;

	bool bHit = UGameplayStatics::PredictProjectilePath(
		this,
		Params,
		Result
	);

	// ===== 点線描画 =====
	for (int32 i = 0; i < Result.PathData.Num() - 1; i++)
	{
		DrawDebugLine(
			GetWorld(),
			Result.PathData[i].Location,
			Result.PathData[i + 1].Location,
			FColor::Green,
			false,
			0.f,
			0,
			2.f
		);
	}

	// ===== 着地点 =====
	/*if (bHit)
	{
		DrawDebugSphere(
			GetWorld(),
			Result.HitResult.Location,
			12.f,
			12,
			FColor::Red,
			false,
			0.f
		);
	}*/
}