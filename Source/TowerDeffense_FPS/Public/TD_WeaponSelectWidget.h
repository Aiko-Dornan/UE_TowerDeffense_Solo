// TD_WeaponSelectWidget.h
#pragma once
#include "Blueprint/UserWidget.h"
#include"WeaponBase.h"
#include "TD_WeaponSelectWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_WeaponSelectWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    UFUNCTION()
    void OnMainWeaponSelected(TSubclassOf<AWeaponBase> WeaponClass);

    UFUNCTION()
    void OnSubWeaponSelected(TSubclassOf<AWeaponBase> WeaponClass);

    UFUNCTION()
    void OnConfirmSelection();

private:
    TSubclassOf<AWeaponBase> TempMainWeapon;
    TSubclassOf<AWeaponBase> TempSubWeapon;
};
