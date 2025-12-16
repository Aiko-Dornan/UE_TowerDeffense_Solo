// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameMode.h"
#include "Blueprint/UserWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void ATitleGameMode::BeginPlay()
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

    if (TitleWidgetClass)
    {
        CreateWidget<UUserWidget>(GetWorld(), TitleWidgetClass)->AddToViewport();
    }
}