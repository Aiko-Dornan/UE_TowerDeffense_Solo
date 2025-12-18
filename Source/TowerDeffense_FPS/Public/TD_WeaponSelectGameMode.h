#pragma once
#include "GameFramework/GameModeBase.h"
#include "TD_WeaponSelectGameMode.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ATD_WeaponSelectGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> WeaponSelectWidgetClass;
};