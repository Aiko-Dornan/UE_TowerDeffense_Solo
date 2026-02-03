// TD_StageSelectWidget.h
#pragma once
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "TD_StageSelectWidget.generated.h"

class USoundBase;

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_StageSelectWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    UFUNCTION()
    void OnStage1Clicked();

    UPROPERTY(meta = (BindWidget))
    UButton* Stage1Button;
    UPROPERTY(meta = (BindWidget))
    UButton* Stage2Button;
    UPROPERTY(meta = (BindWidget))
    UButton* Stage3Button;
    UPROPERTY(meta = (BindWidget))
    UButton* Stage4Button;

    UPROPERTY(meta = (BindWidget))
    UButton* Stage5Button;

    // çƒê∂ÇµÇΩÇ¢SE
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    USoundBase* ClickSE;

    void SEPlay();

    void UpdateStageButtons();
};
