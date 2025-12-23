// TD_TitleWidget.cpp
#include "TitleWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

bool UTitleWidget::Initialize()
{
    Super::Initialize();

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UTitleWidget::OnStartClicked);
    }



    return true;
}

void UTitleWidget::OnStartClicked()
{
    UGameplayStatics::OpenLevel(this, FName("StageSelect"));
    if (ClickSE)
    {
        UGameplayStatics::PlaySound2D(this, ClickSE);
    }
}
