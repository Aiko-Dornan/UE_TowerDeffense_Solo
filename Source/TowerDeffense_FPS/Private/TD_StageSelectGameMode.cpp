// TD_StageSelectGameMode.cpp
#include "TD_StageSelectGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include"TD_GameInstance.h"

//ATD_StageSelectGameMode::ATD_StageSelectGameMode()
//{
//    BGMAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMAudioComponent"));
//    BGMAudioComponent->bAutoActivate = false; // Ž©“®Ä¶OFF
//}

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
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponSelectWidgetClass is NULL"));
    }

    if (UTD_GameInstance* GI = GetGameInstance<UTD_GameInstance>())
    {
        //GI->PlayBGM(SelectBGM, 1.0f);
        GI->PlayBGMByType(EBGMType::Field, 1.0f,false);
    }

   /* if (BGMSound && BGMAudioComponent)
    {

        BGMAudioComponent->SetSound(BGMSound);
        BGMAudioComponent->Play();
        UE_LOG(LogTemp, Warning, TEXT("BGM Start!!"));
    }*/

}
