// TD_StageSelectWidget.cpp
#include "TD_StageSelectWidget.h"
#include "TD_GameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UTD_StageSelectWidget::Initialize()
{
    Super::Initialize();

    /*if (UButton* Btn = Cast<UButton>(GetWidgetFromName(TEXT("Stage1Button"))))
    {
        Btn->OnClicked.AddDynamic(this, &UTD_StageSelectWidget::OnStage1Clicked);
    }
    return true;*/

    if (Stage1Button)
    {
        Stage1Button->OnClicked.AddDynamic(
            this, &UTD_StageSelectWidget::OnStage1Clicked);
    }
    if (Stage2Button)
    {
        Stage2Button->OnClicked.AddDynamic(
            this, &UTD_StageSelectWidget::OnStage1Clicked);
    }
    if (Stage3Button)
    {
        Stage3Button->OnClicked.AddDynamic(
            this, &UTD_StageSelectWidget::OnStage1Clicked);
    }
    if (Stage4Button)
    {
        Stage4Button->OnClicked.AddDynamic(
            this, &UTD_StageSelectWidget::OnStage1Clicked);
    }

    /*if (Stage2Button)
    {
        Stage2Button->OnClicked.AddDynamic(
            this, &UTD_StageSelectWidget::OnStage2Clicked);
    }*/

    // ★ここが重要
    UpdateStageButtons();

    return true;
}

void UTD_StageSelectWidget::OnStage1Clicked()
{
    /*if (UTD_GameInstance* GI = Cast<UTD_GameInstance>(GetGameInstance()))
    {
        GI->SelectedStage = 1;
    }*/

    // 武器選択マップへ
    UGameplayStatics::OpenLevel(this, FName("WeaponSelectMap"));
    SEPlay();
    //UGameplayStatics::OpenLevel(this, FName("BaseMap"));
}

void UTD_StageSelectWidget::SEPlay()
{

    if (ClickSE)
    {
        UGameplayStatics::PlaySound2D(this, ClickSE);
    }

}

void UTD_StageSelectWidget::UpdateStageButtons()
{
    if (UTD_GameInstance* GI = GetGameInstance<UTD_GameInstance>())
    {
        // ステージ1は常に解放
        Stage1Button->SetIsEnabled(true);

        // ステージ2はステージ1クリア後
        Stage2Button->SetIsEnabled(GI->MaxClearedStage >= 1);
        Stage3Button->SetIsEnabled(GI->MaxClearedStage >= 2);
        Stage4Button->SetIsEnabled(GI->MaxClearedStage >= 3);
    }
}
