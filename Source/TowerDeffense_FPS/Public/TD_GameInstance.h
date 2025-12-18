// TD_GameInstance.h
#pragma once
#include "Engine/GameInstance.h"
#include"WeaponBase.h"
#include "WeaponID.h"
#include "TD_GameInstance.generated.h"



UCLASS()
class TOWERDEFFENSE_FPS_API UTD_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    int32 SelectedStage = 0;
    // ===== 選択結果（enum）=====
    UPROPERTY(BlueprintReadWrite)
    EWeaponID SelectedMainWeaponID = EWeaponID::None;

    UPROPERTY(BlueprintReadWrite)
    EWeaponID SelectedSubWeaponID = EWeaponID::None;

    // ===== enum → WeaponClass =====
    TSubclassOf<class AWeaponBase> ResolveWeapon(EWeaponID WeaponID) const;

protected:
    // C++専用（BP不要）
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> AssaultRifleClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> ShotgunClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> PistolClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> SMGClass;
};


