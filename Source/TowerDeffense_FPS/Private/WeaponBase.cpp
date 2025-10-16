//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "WeaponBase.h"
//#include"Kismet/GameplayStatics.h"
//#include"Kismet/KismetMathLibrary.h"
//
//
//// Sets default values
//AWeaponBase::AWeaponBase()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//	PrimaryActorTick.bStartWithTickEnabled = false;
//
//	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
//	RootComponent = Mesh;
//
//	bCanFire = true;
//
//	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
//
//	MaxAmmo = 30.0f;
//	Ammo = 30.0f;
//	StockAmmo = 300.0f;
//	WeaponDamage = 15.0f;
//	FireRate = 0.5f;
//	FireRange = 3000.0f;
//	FireSpread = 2.5f;
//
//}
//
//// Called when the game starts or when spawned
//void AWeaponBase::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//
//
//
