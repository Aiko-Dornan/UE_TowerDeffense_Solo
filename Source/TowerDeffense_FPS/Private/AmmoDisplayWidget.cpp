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
    UE_LOG(LogTemp, Warning, TEXT("UpdateAmmoText called: %d / %d"), CurrentAmmo, StockAmmo);
}

void UAmmoDisplay::SetReloading(bool bIsReloading)
{
    if (AmmoTextBlock)
    {
        AmmoTextBlock->SetVisibility(bIsReloading ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
    }

    if (ReloadTextBlock)
    {
        ReloadTextBlock->SetVisibility(bIsReloading ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        if (bIsReloading)
        {
            ReloadTextBlock->SetText(FText::FromString(TEXT("RELOADING...")));
        }
    }
}