// TD_TitleGameMode.h
#pragma once
#include "GameFramework/GameModeBase.h"
#include "TitleGameMode.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ATitleGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> TitleWidgetClass;
};
