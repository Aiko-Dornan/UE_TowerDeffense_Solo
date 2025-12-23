// TD_TitleGameMode.h
#pragma once
#include "GameFramework/GameModeBase.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TitleGameMode.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ATitleGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
    USoundBase* TitleBGM;

  /*  UPROPERTY()
    UAudioComponent* BGMAudioComponent;*/

public:

   // ATitleGameMode();

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> TitleWidgetClass;
};
