// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include"WeaponBase.h"
#include "GameFramework/Character.h"
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

	// �G�f�B�^����w��\�ȕ���N���X
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeaponBase> GunComponent;

	// ���ݑ������̕���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeaponBase* CurrentWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	// ������X�|�[������������֐�
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> AmmoWidgetClass;

	//Getter

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentStockAmmo() const;


};
