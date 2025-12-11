// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventorySlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/PanelWidget.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!SlotContainer || !SlotWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("SlotContainer or SlotWidgetClass is NULL!"));
        return;
    }

    // 既存スロットをクリア
    SlotWidgets.Empty();
    SlotContainer->ClearChildren();

    //// SlotCount 個のスロットを自動生成
    //for (int32 i = 0; i < SlotCount; i++)
    //{
    //    UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);

    //    if (NewSlot)
    //    {
    //        SlotContainer->AddChild(NewSlot);
    //        SlotWidgets.Add(NewSlot);
    //    }
    //}

    FVector2D StartOffset(300.f, 0.f); // 横に50pxずらしたい場合

    // HorizontalBoxの先頭にSpacerを追加
    if (SlotContainer)
    {
        UHorizontalBox* HBox = Cast<UHorizontalBox>(SlotContainer);
        if (HBox)
        {
            // Spacer生成
            USpacer* Spacer = NewObject<USpacer>(this, USpacer::StaticClass());
            Spacer->SetSize(StartOffset);
            HBox->AddChild(Spacer);
        }
    }

    // スロットを追加
    for (int32 i = 0; i < SlotCount; i++)
    {
        UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
        if (NewSlot && SlotContainer)
        {
            UHorizontalBox* HBox = Cast<UHorizontalBox>(SlotContainer);
            if (HBox)
            {
                HBox->AddChild(NewSlot);
            }
            SlotWidgets.Add(NewSlot);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Generated %d Inventory Slots"), SlotWidgets.Num());
}


void UInventoryWidget::UpdateInventory(const TArray<FInventorySlot>& Inventory)
{
    for (int32 i = 0; i < SlotWidgets.Num(); i++)
    {
        UInventorySlotWidget* SlotW = SlotWidgets[i];
        if (!SlotW) continue;

        if (!Inventory.IsValidIndex(i) || Inventory[i].IsEmpty())
        {
            SlotW->UpdateSlot(nullptr, 0, TEXT(""));
        }
        else
        {
            const FInventorySlot& SlotData = Inventory[i];
            AItemBase* ItemCDO = SlotData.ItemClass->GetDefaultObject<AItemBase>();

            FString ItemName = ItemCDO->GetName(); // または ItemCDO->CustomName を使う

            SlotW->UpdateSlot(ItemCDO->ItemIcon, SlotData.Quantity, ItemName);
        }
    }
}