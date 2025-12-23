// TD_StageSelectGameMode.h
#pragma once
#include "GameFramework/GameModeBase.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TD_StageSelectGameMode.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ATD_StageSelectGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
    USoundBase* SelectBGM;

    //UPROPERTY()
    //UAudioComponent* BGMAudioComponent;

public:
    //ATD_StageSelectGameMode();

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> StageSelectWidgetClass;
};
