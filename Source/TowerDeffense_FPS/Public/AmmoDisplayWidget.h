// AmmoDisplay.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
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

    //追加：HPゲージのProgressBar
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HPBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPTextBlock;

    float CHP = 100.f;
    float MHP = 100.f;

    void NativeConstruct();

    // 弾数更新関数
    UFUNCTION(BlueprintCallable)
    void UpdateAmmoText(int32 CurrentAmmo, int32 StockAmmo);

    // リロード状態切替
    UFUNCTION(BlueprintCallable)
    void SetReloading(bool bIsReloading);

    // HP更新関数
    UFUNCTION(BlueprintCallable)
    void UpdateHP(float CurrentHP, float MaxHP);
};