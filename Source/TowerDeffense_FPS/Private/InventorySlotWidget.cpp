#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::UpdateSlot(UTexture2D* ItemIcon, int32 Quantity, const FString& ItemName)
{

   

    if (SlotImage)
    {
        if (ItemIcon)
        {
            SlotImage->SetBrushFromTexture(ItemIcon);
            SlotImage->SetVisibility(ESlateVisibility::Visible);  // ← アイコン表示
        }
        else
        {
            SlotImage->SetBrushFromTexture(nullptr);
            SlotImage->SetVisibility(ESlateVisibility::Hidden);   // ← 透明（非表示）
        }
    }

    if (QuantityText)
    {
        if (Quantity > 0)
            QuantityText->SetText(FText::AsNumber(Quantity));
        else
            QuantityText->SetText(FText::GetEmpty());
    }

    if (ItemNameText)
    {
        if (!ItemName.IsEmpty())
            ItemNameText->SetText(FText::FromString(ItemName));
        else
            ItemNameText->SetText(FText::GetEmpty());
    }
}


void UInventorySlotWidget::SetHighlighted(bool bHighlight)
{
    if (Image_Highlight)
    {
        Image_Highlight->SetVisibility(
            bHighlight ? ESlateVisibility::Visible : ESlateVisibility::Hidden
        );
    }

   /* if (SlotBorder)
    {
        FLinearColor Color = bHighlight ? FLinearColor::White : FLinearColor::Black;
        SlotBorder->SetBrushColor(Color);
    }*/
}
