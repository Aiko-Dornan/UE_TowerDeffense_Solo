// TD_TitleWidget.cpp
#include "TD_TitleWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UTD_TitleWidget::Initialize()
{
    Super::Initialize();

    if (UButton* Btn = Cast<UButton>(GetWidgetFromName(TEXT("StartButton"))))
    {
        Btn->OnClicked.AddDynamic(this, &UTD_TitleWidget::OnStartClicked);
    }
    return true;
}

void UTD_TitleWidget::OnStartClicked()
{
    UGameplayStatics::OpenLevel(this, FName("StageSelectMap"));
}
