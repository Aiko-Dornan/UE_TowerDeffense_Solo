// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHeroPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h" // 忘れずに！

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

	//GunComponent = CreateDefaultSubobject<AWeaponBase>(TEXT("/Game/BluePrint/MyWeaponBase"));

	CurrentWeapon = nullptr;

}

// Called when the game starts or when spawned
void AMyHeroPlayer::BeginPlay()
{
	Super::BeginPlay();

	// ゲーム開始時に銃を装備
	if (GunComponent)
	{
		EquipWeapon(GunComponent);
	}

	if (AmmoWidgetClass)
	{
		UUserWidget* AmmoWidget = CreateWidget<UUserWidget>(GetWorld(), AmmoWidgetClass);
		if (AmmoWidget)
		{
			AmmoWidget->AddToViewport();
		}
	}

}

// Called every frame
void AMyHeroPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyHeroPlayer::HandleFire);
	// クリックで射撃
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyHeroPlayer::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMyHeroPlayer::OnFireReleased);
	//reload
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMyHeroPlayer::OnReloadPressed);

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
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire(); // ← 押した瞬間
	}
}

void AMyHeroPlayer::OnFireReleased()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire(); // ← 離した瞬間
	}
}

void AMyHeroPlayer::OnReloadPressed()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
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

