// InventorySlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"InventorySlot.h"
#include "InventorySlotWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    class UImage* SlotImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* QuantityText;

    UFUNCTION(BlueprintCallable)
    void UpdateSlot(UTexture2D* ItemIcon, int32 Quantity);
};
