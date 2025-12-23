// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameMode.h"
#include "Blueprint/UserWidget.h"
#include"TD_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

//ATitleGameMode::ATitleGameMode()
//{
//    BGMAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMAudioComponent"));
//    BGMAudioComponent->bAutoActivate = false; // Ž©“®Ä¶OFF
//}

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

    if (UTD_GameInstance* GI = GetGameInstance<UTD_GameInstance>())
    {
        /*GI->PlayBGM(TitleBGM, 1.0f);*/
        GI->PlayBGMByType(EBGMType::Title,1.0f,true);
        UE_LOG(LogTemp, Warning, TEXT("BGM Start!!"));
    }

  /*  if (BGMSound && BGMAudioComponent)
    {
        
        BGMAudioComponent->SetSound(BGMSound);
        BGMAudioComponent->Play();
        UE_LOG(LogTemp, Warning, TEXT("BGM Start!!"));
    }*/

}