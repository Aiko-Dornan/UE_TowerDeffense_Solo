// TD_TitleWidget.cpp
#include "TitleWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

//bool UTitleWidget::Initialize()
//{
//    Super::Initialize();
//
//    if (StartButton)
//    {
//        StartButton->OnClicked.AddDynamic(this, &UTitleWidget::OnStartClicked);
//    }
//
//
//
//    return true;
//}

void UTitleWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (StartButton)
            {
                StartButton->OnClicked.AddDynamic(this, &UTitleWidget::OnStartClicked);
            }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UTitleWidget::OnQuitClicked);
    }
}

void UTitleWidget::OnStartClicked()
{
    if (ClickSE)
    {
        UGameplayStatics::PlaySound2D(this, ClickSE);
    }
    UGameplayStatics::OpenLevel(this, FName("StageSelect"));
    
}

void UTitleWidget::OnQuitClicked()
{
    if (ClickSE)
    {
        UGameplayStatics::PlaySound2D(this, ClickSE);
    }


    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        UKismetSystemLibrary::QuitGame(
            GetWorld(),
            PC,
            EQuitPreference::Quit,
            false
        );
    }

  /*  UE_LOG(LogTemp, Warning, TEXT("Quit button clicked"));
 
    UKismetSystemLibrary::QuitGame(
        GetWorld(),
        GetOwningPlayer(),
        EQuitPreference::Quit,
        false
    );*/
}