#include "AmmoDisplayWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MyHeroPlayer.h"

void UAmmoDisplay::UpdateAmmoText(int32 CurrentAmmo, int32 StockAmmo)
{
    if (AmmoTextBlock)
    {
        FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, StockAmmo);
        AmmoTextBlock->SetText(FText::FromString(AmmoString));
    }
}

void UAmmoDisplay::SetReloading(bool bIsReloading)
{
    /*if (ReloadTextBlock)
    {
        ReloadTextBlock->SetVisibility(bIsReloading ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }*/

    // リロード中は弾数表示を非表示にしたい場合
    if (AmmoTextBlock)
    {
        AmmoTextBlock->SetVisibility(bIsReloading ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }
}