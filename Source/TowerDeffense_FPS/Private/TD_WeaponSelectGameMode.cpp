// TD_WeaponSelectGameMode.cpp
#include "TD_WeaponSelectGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ATD_WeaponSelectGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        PC->bShowMouseCursor = true;
        PC->SetInputMode(FInputModeUIOnly());
    }

    if (WeaponSelectWidgetClass)
    {
        CreateWidget<UUserWidget>(GetWorld(), WeaponSelectWidgetClass)->AddToViewport();
    }
}
