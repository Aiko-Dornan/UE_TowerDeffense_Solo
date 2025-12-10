// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"InventorySlotWidget.h"
#include "InventoryWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // スロット一覧（Blueprint で埋める）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
    TArray<UInventorySlotWidget*> SlotWidgets;

    UFUNCTION(BlueprintCallable)
    void UpdateInventory(const TArray<FInventorySlot>& Inventory);
};

