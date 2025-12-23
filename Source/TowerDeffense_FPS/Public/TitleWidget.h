// TD_TitleWidget.h
#pragma once
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "TitleWidget.generated.h"

class USoundBase;

UCLASS()
class TOWERDEFFENSE_FPS_API UTitleWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    UFUNCTION()
    void OnStartClicked();

    // çƒê∂ÇµÇΩÇ¢SE
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    USoundBase* ClickSE;

public:
    UPROPERTY(meta = (BindWidget))
    UButton* StartButton;
};
