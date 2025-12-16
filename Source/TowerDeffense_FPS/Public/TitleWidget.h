// TD_TitleWidget.h
#pragma once
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "TitleWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UTitleWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    UFUNCTION()
    void OnStartClicked();

public:
    UPROPERTY(meta = (BindWidget))
    UButton* StartButton;
};
