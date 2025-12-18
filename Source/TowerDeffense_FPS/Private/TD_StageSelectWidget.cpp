// TD_StageSelectWidget.cpp
#include "TD_StageSelectWidget.h"
#include "TD_GameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UTD_StageSelectWidget::Initialize()
{
    Super::Initialize();

    if (UButton* Btn = Cast<UButton>(GetWidgetFromName(TEXT("Stage1Button"))))
    {
        Btn->OnClicked.AddDynamic(this, &UTD_StageSelectWidget::OnStage1Clicked);
    }
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

    //UGameplayStatics::OpenLevel(this, FName("BaseMap"));
}
