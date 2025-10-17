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

	
	TSubclassOf<class AWeaponBase> GunComponent;

	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyHeroPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
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

	// ïêäÌÇÉXÉ|Å[ÉìÅïëïîıÇ∑ÇÈä÷êî
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass);
};
