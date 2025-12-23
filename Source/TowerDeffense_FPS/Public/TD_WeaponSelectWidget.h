#pragma once
#include "Blueprint/UserWidget.h"
#include "WeaponID.h"
#include "TD_WeaponSelectWidget.generated.h"

class UButton;
class USoundBase;

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_WeaponSelectWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual bool Initialize() override;

    // ===== BindWidget =====
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Main_AR;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Main_SG;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Sub_Pistol;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Sub_SMG;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Confirm;

    // çƒê∂ÇµÇΩÇ¢SE
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    USoundBase* ClickSE;

    void SEPlay();

private:
    // ===== ëIëèÛë‘ =====
    EWeaponID TempMainWeaponID = EWeaponID::AssaultRifle;
    EWeaponID TempSubWeaponID = EWeaponID::Pistol;

    // ===== ÉnÉìÉhÉâ =====
    UFUNCTION()
    void OnMainAR();

    UFUNCTION()
    void OnMainShotgun();

    UFUNCTION()
    void OnSubPistol();

    UFUNCTION()
    void OnSubSMG();

    UFUNCTION()
    void OnConfirm();

    void UpdateButtonVisuals();
};
