// TD_StageSelectWidget.h
#pragma once
#include "Blueprint/UserWidget.h"
#include "TD_StageSelectWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_StageSelectWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    UFUNCTION()
    void OnStage1Clicked();
};
