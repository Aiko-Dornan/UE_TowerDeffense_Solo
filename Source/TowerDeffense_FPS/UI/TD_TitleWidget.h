// TD_TitleWidget.h
#pragma once
#include "Blueprint/UserWidget.h"
#include "TD_TitleWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_TitleWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    UFUNCTION()
    void OnStartClicked();
};
