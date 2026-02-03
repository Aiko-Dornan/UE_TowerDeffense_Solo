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
    Clear,
    Failed,
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

    //// 現在のBGM取得
    //USoundBase* GetCurrentBGM() const { return CurrentBGM; }


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

    void PlayBGMByType(EBGMType Type, float FadeInTime, bool bForceRestart=false);

    //ステージクリア判定
     // クリア済みの最大ステージ番号
    UPROPERTY(BlueprintReadWrite)
    int32 MaxClearedStage = 0;
    int NowClearStage = 0;
    bool ClearFlag[4]{ false };
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage")
    TArray<bool> BossSummon;
    // ステージクリア時に呼ぶ
    void SetStageCleared(int32 StageNum);

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

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> SRClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> HSGClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> SemiSGClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> UziClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AWeaponBase> GLClass;

    //bgm
    UPROPERTY()
    UAudioComponent* BGMAudioComponent;

    UPROPERTY()
    USoundBase* CurrentBGM;

    

private:
    void HandleWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);


};


