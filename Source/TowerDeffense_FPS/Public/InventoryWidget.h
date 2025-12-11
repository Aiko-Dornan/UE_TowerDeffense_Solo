// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"InventorySlotWidget.h"
#include "Components/PanelWidget.h"
#include "InventoryWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // UMG に置くコンテナ（VerticalBox / GridPanel など）
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* SlotContainer;

    // スロットWidgetのクラス（BPで設定）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

    // 自動生成されたスロット一覧
    UPROPERTY()
    TArray<UInventorySlotWidget*> SlotWidgets;

    // スロットを何個生成するか
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 SlotCount = 20;

    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void UpdateInventory(const TArray<FInventorySlot>& Inventory);
};

