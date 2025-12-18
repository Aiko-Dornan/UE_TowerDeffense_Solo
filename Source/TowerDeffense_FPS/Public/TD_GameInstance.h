// TD_GameInstance.h
#pragma once
#include "Engine/GameInstance.h"
#include"WeaponBase.h"
#include "TD_GameInstance.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    int32 SelectedStage = 0;

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<AWeaponBase> SelectedMainWeapon;

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<AWeaponBase> SelectedSubWeapon;

};
