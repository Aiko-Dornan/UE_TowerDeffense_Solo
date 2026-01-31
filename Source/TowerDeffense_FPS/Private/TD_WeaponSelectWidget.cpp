#include "TD_WeaponSelectWidget.h"
#include "Components/Button.h"
#include "TD_GameInstance.h"
#include "Kismet/GameplayStatics.h"

bool UTD_WeaponSelectWidget::Initialize()
{
    Super::Initialize();

    UpdateButtonVisuals();

    if (Btn_Main_AR)
        Btn_Main_AR->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnMainAR);

    if (Btn_Main_SG)
        Btn_Main_SG->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnMainShotgun);
    if (Btn_Main_SR)
        Btn_Main_SR->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnMainSR);
    if (Btn_Main_SMG)
        Btn_Main_SMG->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnMainSMG);

    if (Btn_Sub_Pistol)
        Btn_Sub_Pistol->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnSubPistol);

   

    if (Btn_Sub_HSG)
        Btn_Sub_HSG->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnSubHSG);

    if (Btn_Confirm)
        Btn_Confirm->OnClicked.AddDynamic(this, &UTD_WeaponSelectWidget::OnConfirm);

    return true;
}

void UTD_WeaponSelectWidget::OnMainAR()
{
    TempMainWeaponID = EWeaponID::AssaultRifle;
    UpdateButtonVisuals();
}

void UTD_WeaponSelectWidget::OnMainShotgun()
{
    TempMainWeaponID = EWeaponID::Shotgun;
    UpdateButtonVisuals();
}

void UTD_WeaponSelectWidget::OnMainSR()
{
    TempMainWeaponID = EWeaponID::SR;
    UpdateButtonVisuals();
}

void UTD_WeaponSelectWidget::OnMainSMG()
{
    TempMainWeaponID = EWeaponID::SMG;
    UpdateButtonVisuals();
}


void UTD_WeaponSelectWidget::OnSubPistol()
{
    TempSubWeaponID = EWeaponID::Pistol;
    UpdateButtonVisuals();
}

void UTD_WeaponSelectWidget::OnSubHSG()
{
    TempSubWeaponID = EWeaponID::HSG;
    UpdateButtonVisuals();
}

void UTD_WeaponSelectWidget::OnConfirm()
{
    if (UTD_GameInstance* GI = Cast<UTD_GameInstance>(GetGameInstance()))
    {
        GI->SelectedMainWeaponID = TempMainWeaponID;
        GI->SelectedSubWeaponID = TempSubWeaponID;
        GI->SelectedStage = 1;
    }

    UGameplayStatics::OpenLevel(this, FName("BaseMap"));
}

void UTD_WeaponSelectWidget::UpdateButtonVisuals()
{
    // 色を決める
    FLinearColor SelectedColor(0.f, 1.f, 0.f, 1.f); // 緑
    FLinearColor DefaultColor(1.f, 1.f, 1.f, 1.f);  // 白

    // メイン武器
    if (Btn_Main_AR)
        Btn_Main_AR->SetBackgroundColor(TempMainWeaponID == EWeaponID::AssaultRifle ? SelectedColor : DefaultColor);
    if (Btn_Main_SG)
        Btn_Main_SG->SetBackgroundColor(TempMainWeaponID == EWeaponID::Shotgun ? SelectedColor : DefaultColor);
    if (Btn_Main_SR)
        Btn_Main_SR->SetBackgroundColor(TempMainWeaponID == EWeaponID::SR ? SelectedColor : DefaultColor);
    if (Btn_Main_SMG)
        Btn_Main_SMG->SetBackgroundColor(TempMainWeaponID == EWeaponID::SMG ? SelectedColor : DefaultColor);


    // サブ武器
    if (Btn_Sub_Pistol)
        Btn_Sub_Pistol->SetBackgroundColor(TempSubWeaponID == EWeaponID::Pistol ? SelectedColor : DefaultColor);
   
    if (Btn_Sub_HSG)
        Btn_Sub_HSG->SetBackgroundColor(TempSubWeaponID == EWeaponID::HSG ? SelectedColor : DefaultColor);
    
    SEPlay();
}

void UTD_WeaponSelectWidget::SEPlay()
{

    if (ClickSE)
    {
        UGameplayStatics::PlaySound2D(this, ClickSE);
    }

}