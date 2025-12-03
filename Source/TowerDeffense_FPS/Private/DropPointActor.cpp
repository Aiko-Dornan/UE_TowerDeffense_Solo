// Fill out your copyright notice in the Description page of Project Settings.


#include "DropPointActor.h"

#include "Components/SphereComponent.h"

ADropPointActor::ADropPointActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // デバッグ用に小さなSphereを表示
    SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComponent"));
    RootComponent = SphereComponent;

   
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // エディタ上で見えるようにする（ゲーム中非表示）
    SphereComponent->SetHiddenInGame(true);

    // タグを付ける（Drone が取得できるように）
    Tags.Add(FName("DropTargetPoint"));
}

void ADropPointActor::BeginPlay()
{
    Super::BeginPlay();
}

void ADropPointActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

