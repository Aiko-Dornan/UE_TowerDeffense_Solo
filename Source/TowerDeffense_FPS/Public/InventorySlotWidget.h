// InventorySlotWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"InventorySlot.h"
#include "Components/Border.h"
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

    // アイテム名表示（新規追加）
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ItemNameText;

    // スロット更新関数
    UFUNCTION(BlueprintCallable)
    void UpdateSlot(UTexture2D* ItemIcon, int32 Quantity, const FString& ItemName);

    UFUNCTION(BlueprintCallable)
    void SetHighlighted(bool bHighlight);

    UPROPERTY(meta = (BindWidget))
    class UImage* Image_Highlight; // 選択中のハイライト画像（初期Hidden）
    UPROPERTY(meta = (BindWidget))
    class UImage* Image_Frame;     // 枠線（常時表示）
};
