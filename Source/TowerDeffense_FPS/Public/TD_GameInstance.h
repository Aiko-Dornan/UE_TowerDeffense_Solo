// TD_GameInstance.h
#pragma once
#include "Engine/GameInstance.h"
#include"WeaponBase.h"
#include "WeaponID.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TD_GameInstance.generated.h"

UENUM(BlueprintType)
enum class EBGMType : uint8
{
    None,
    Title,
    Field,
    Battle,
};

UCLASS()
class TOWERDEFFENSE_FPS_API UTD_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:

    virtual void Init() override;
    virtual void Shutdown() override;
    // BGM操作API
    UFUNCTION(BlueprintCallable)
    void PlayBGM(USoundBase* NewBGM, float FadeInTime = 1.0f);

   /* UFUNCTION(BlueprintCallable)
    void StopBGM(float FadeOutTime = 1.0f);*/

    UPROPERTY(BlueprintReadWrite)
    int32 SelectedStage = 0;
    // ===== 選択結果（enum）=====
    UPROPERTY(BlueprintReadWrite)
    EWeaponID SelectedMainWeaponID = EWeaponID::None;

    UPROPERTY(BlueprintReadWrite)
    EWeaponID SelectedSubWeaponID = EWeaponID::None;

    // ===== enum → WeaponClass =====
    TSubclassOf<class AWeaponBase> ResolveWeapon(EWeaponID WeaponID) const;

    

    UPROPERTY()
    EBGMType CurrentBGMType = EBGMType::None;

    UPROPERTY(EditDefaultsOnly)
    TMap<EBGMType, USoundBase*> BGMTable;

    void PlayBGMByType(EBGMType Type, float FadeInTime);

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

    //bgm
    UPROPERTY()
    UAudioComponent* BGMAudioComponent;

    UPROPERTY()
    USoundBase* CurrentBGM;

private:
    void HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);


};


