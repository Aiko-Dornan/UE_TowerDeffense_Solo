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
    GEngine->AddOnScreenDebugMessage(
        -1, 5.f, FColor::Green, TEXT("Weapon Select UI Test")
    );
    if (WeaponSelectWidgetClass)
    {
        CreateWidget<UTD_WeaponSelectWidget>(GetWorld(), WeaponSelectWidgetClass)->AddToViewport();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponSelectWidgetClass is NULL"));
    }
}
