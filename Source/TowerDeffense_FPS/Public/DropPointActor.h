// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DropPointActor.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ADropPointActor : public AActor
{
    GENERATED_BODY()

public:
    // コンストラクタ
    ADropPointActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // デバッグ用に表示するメッシュ（任意）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    class UStaticMeshComponent* SphereComponent;

    // ターゲットにする優先順位（任意で利用可能）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
    int32 TargetOrder = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
    bool bAlreadyDropped = false;
};
