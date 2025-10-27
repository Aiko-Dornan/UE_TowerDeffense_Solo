// AmmoDisplay.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoDisplayWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UAmmoDisplay : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmmoTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReloadTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPTextBlock;

    // 弾数更新関数
    UFUNCTION(BlueprintCallable)
    void UpdateAmmoText(int32 CurrentAmmo, int32 StockAmmo);

    // リロード状態切替
    UFUNCTION(BlueprintCallable)
    void SetReloading(bool bIsReloading);

    // HP更新関数
    UFUNCTION(BlueprintCallable)
    void UpdateHPText(int32 RestHP);
};