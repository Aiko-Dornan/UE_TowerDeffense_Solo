#include "AmmoDisplayWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MyHeroPlayer.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "AmmoDisplayWidget"



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

void UAmmoDisplay::NativeConstruct()
{
    Super::NativeConstruct();

    // ここで初期値をセット
    UpdateHP(CHP, MHP);
}

void UAmmoDisplay::UpdateHP(float CurrentHP, float MaxHP)
{
    if (HPBar)
    {
        float Ratio = CurrentHP / MaxHP;
        HPBar->SetPercent(Ratio);
    }

    if (HPTextBlock)
    {
        /*FString HPString = FString::Printf(TEXT("%f"), CurrentHP);
        HPTextBlock->SetText(FText::FromString(HPString));*/
        HPTextBlock->SetText(FText::AsNumber(FMath::RoundToInt(CurrentHP)));
    }
}

void UAmmoDisplay::UpdateDroneText(int Case)
{
    if (!DroneTextBlock)
    {
        return;
    }

    switch (Case)
    {
    case 0: // die
        DroneTextBlock->SetText(
            LOCTEXT("DroneDie", "補給ドローンが墜落しました...")
        );
        break;

    case 1: // spawn
        DroneTextBlock->SetText(
            LOCTEXT("DroneSpawn", "補給ドローンが前線に到着しました")
        );
        break;

    case 2: // arrive
        DroneTextBlock->SetText(
            LOCTEXT("DroneArrive", "補給ドローンが物資を投下しました")
        );
        break;
    case 3: //ally die
        DroneTextBlock->SetText(
            LOCTEXT("DroneArrive", "味方が倒されました...")
        );
        break;
    case 4: //enemy spawn
        DroneTextBlock->SetText(
            LOCTEXT("DroneArrive", "敵が出現しました!")
        );
        break;
    case 5: //enemy spawn
        DroneTextBlock->SetText(
            LOCTEXT("DroneArrive", "バリケードが破壊されました!")
        );
        break;
    case 6: //enemy spawn
        DroneTextBlock->SetText(
            LOCTEXT("DroneArrive", "最終ウェーブです!")
        );
        break;
    default:
        DroneTextBlock->SetText(FText::GetEmpty());
        break;
    }

    GetWorld()->GetTimerManager().SetTimer(
        UpdateTextTimerHandle,
        this,
        &UAmmoDisplay::ClearDroneText,
        UpdateTextInterval,
        true
    );

}

void UAmmoDisplay::ClearDroneText()
{
    if (DroneTextBlock)
    {
        DroneTextBlock->SetText(FText::GetEmpty());
    }
}

#undef LOCTEXT_NAMESPACE