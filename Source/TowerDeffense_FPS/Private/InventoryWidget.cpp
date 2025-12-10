// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventorySlotWidget.h"

void UInventoryWidget::UpdateInventory(const TArray<FInventorySlot>& Inventory)
{
    for (int32 i = 0; i < SlotWidgets.Num(); i++)
    {
        if (!SlotWidgets[i]) continue;

        if (!Inventory.IsValidIndex(i) || Inventory[i].IsEmpty())
        {
            SlotWidgets[i]->UpdateSlot(nullptr, 0);
        }
        else
        {
            const FInventorySlot& SlotData = Inventory[i];
            AItemBase* DefaultItem = SlotData.ItemClass->GetDefaultObject<AItemBase>();

            SlotWidgets[i]->UpdateSlot(DefaultItem->ItemIcon, SlotData.Quantity);
        }
    }
}
