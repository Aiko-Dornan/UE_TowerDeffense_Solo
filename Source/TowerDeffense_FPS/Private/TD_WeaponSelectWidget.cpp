// TD_WeaponSelectWidget.cpp
#include "TD_WeaponSelectWidget.h"
#include "TD_GameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UTD_WeaponSelectWidget::Initialize()
{
    Super::Initialize();

    // ボタン紐付け（例）
    // MainWeaponButton->OnClicked.AddDynamic(...)
    // SubWeaponButton->OnClicked.AddDynamic(...)
    // ConfirmButton->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnConfirmClicked);

    return true;
}

void UTD_WeaponSelectWidget::OnMainWeaponSelected(TSubclassOf<AWeaponBase> MainWeapon)
{
    TempMainWeapon = 0; // IDなど
}

void UTD_WeaponSelectWidget::OnSubWeaponSelected(TSubclassOf<AWeaponBase> SubWeapon)
{
    TempSubWeapon = 0;
}

void UTD_WeaponSelectWidget::OnConfirmSelection()
{
    if (UTD_GameInstance* GI = Cast<UTD_GameInstance>(GetGameInstance()))
    {
        GI->SelectedMainWeapon = TempMainWeapon;
        GI->SelectedSubWeapon = TempSubWeapon;
    }

    // 選択後にゲームマップをロード
    UGameplayStatics::OpenLevel(this, FName("BaseMap"));
}
