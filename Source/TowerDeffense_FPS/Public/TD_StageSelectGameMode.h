// TD_StageSelectGameMode.h
#pragma once
#include "GameFramework/GameModeBase.h"
#include "TD_StageSelectGameMode.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ATD_StageSelectGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> StageSelectWidgetClass;
};
