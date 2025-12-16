// TD_StageSelectGameMode.cpp
#include "TD_StageSelectGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


void ATD_StageSelectGameMode::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->bShowMouseCursor = true;

        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
    }

    if (StageSelectWidgetClass)
    {
        CreateWidget<UUserWidget>(GetWorld(), StageSelectWidgetClass)->AddToViewport();
    }
}
