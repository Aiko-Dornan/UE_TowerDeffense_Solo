#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::UpdateSlot(UTexture2D* ItemIcon, int32 Quantity)
{
    if (SlotImage)
    {
        if (ItemIcon)
            SlotImage->SetBrushFromTexture(ItemIcon);
        else
            SlotImage->SetBrushFromTexture(nullptr);
    }

    if (QuantityText)
    {
        if (Quantity > 1)
            QuantityText->SetText(FText::AsNumber(Quantity));
        else
            QuantityText->SetText(FText::GetEmpty());
    }
}
