// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "InventorySlot.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AItemBase> ItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    FInventorySlot()
        : ItemClass(nullptr)
        , Quantity(0)
    {
    }

    bool IsEmpty() const { return ItemClass == nullptr || Quantity <= 0; }
};
//UCLASS()
//class TOWERDEFFENSE_FPS_API UInventorySlot
//{
//    GENERATED_BODY()
//
//public:
//    UInventorySlot();
//};