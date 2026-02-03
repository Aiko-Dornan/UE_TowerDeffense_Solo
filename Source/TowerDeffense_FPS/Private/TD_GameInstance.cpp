//#include "TD_GameInstance.h"
//#include "WeaponBase.h"
//
//void UTD_GameInstance::Init()
//{
//    Super::Init();
//
//    BGMAudioComponent = NewObject<UAudioComponent>(this);
//    BGMAudioComponent->bAutoActivate = false;
//    BGMAudioComponent->bIsUISound = true;
//    BGMAudioComponent->bStopWhenOwnerDestroyed = false;
//    BGMAudioComponent->bAutoDestroy = false;
//    // ★ World破棄を監視
//    FWorldDelegates::OnWorldCleanup.AddUObject(
//        this, &UTD_GameInstance::HandleWorldCleanup);
//
//    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
//}
//
//void UTD_GameInstance::Shutdown()
//{
//    FWorldDelegates::OnWorldCleanup.RemoveAll(this);
//    Super::Shutdown();
//}
//
//void UTD_GameInstance::HandleWorldCleanup(
//    UWorld* World, bool bSessionEnded, bool bCleanupResources)
//{
//    if (BGMAudioComponent && BGMAudioComponent->IsRegistered())
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Unregister AudioComponent (World Cleanup)"));
//        BGMAudioComponent->Stop();
//        BGMAudioComponent->UnregisterComponent();
//    }
//}
//
//void UTD_GameInstance::PlayBGM(USoundBase* NewBGM, float FadeInTime)
//{
//    if (!NewBGM || !BGMAudioComponent)
//        return;
//
//    // ★ ここがポイント
//    if (CurrentBGM == NewBGM && BGMAudioComponent->IsPlaying())
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Same BGM - continue playing"));
//        return;
//    }
//    else
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Different BGM,%s"),*NewBGM->GetName());
//    }
//
//    CurrentBGM = NewBGM;
//
//    // ★ Worldが変わっていたら再登録
//    if (!BGMAudioComponent->IsRegistered())
//    {
//        BGMAudioComponent->RegisterComponentWithWorld(GetWorld());
//        UE_LOG(LogTemp, Warning, TEXT("AudioComponent Registered With New World"));
//    }
//
//    BGMAudioComponent->SetSound(NewBGM);
//    BGMAudioComponent->FadeIn(FadeInTime, 1.0f);
//}
//
//void UTD_GameInstance::PlayBGMByType(EBGMType Type, float FadeInTime, bool bForceRestart)
//{
//    //if (CurrentBGMType == Type && BGMAudioComponent->IsPlaying())
//    //    return;
//
//    //USoundBase** Found = BGMTable.Find(Type);
//    //if (!Found) return;
//
//    //if (BGMAudioComponent)
//    //{
//    //    BGMAudioComponent->bIsUISound = true;
//    //    BGMAudioComponent->bAutoDestroy = false;
//    //   // BGMAudioComponent->SetVolumeMultiplier(0.5f);
//    //}
//
//    //CurrentBGMType = Type;
//    //PlayBGM(*Found, FadeInTime);
//
//    // すでに同じBGMが再生中 ＆ 再スタート不要なら何もしない
//    if (!bForceRestart &&
//        BGMAudioComponent &&
//        BGMAudioComponent->IsPlaying() &&
//        CurrentBGMType == Type)
//    {
//        return;
//    }
//
//    USoundBase** Found = BGMTable.Find(Type);
//    if (!Found || !(*Found))
//    {
//        return;
//    }
//
//    // 既存BGMがあればフェードアウト
//    if (BGMAudioComponent)
//    {
//        BGMAudioComponent->FadeOut(0.5f, 0.0f);
//        BGMAudioComponent = nullptr;
//    }
//
//    CurrentBGMType = Type;
//
//    // 再生（PlayBGM 内で SpawnSound2D している想定）
//    PlayBGM(*Found, FadeInTime);
//
//    // 再生後の AudioComponent 設定
//    if (BGMAudioComponent)
//    {
//        BGMAudioComponent->bIsUISound = true;
//        BGMAudioComponent->bAutoDestroy = false;
//        // BGMAudioComponent->SetVolumeMultiplier(0.5f);
//    }
//
//}

#include "TD_GameInstance.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void UTD_GameInstance::Init()
{
    Super::Init();

    BGMAudioComponent = NewObject<UAudioComponent>(this);
    BGMAudioComponent->bAutoActivate = false;
    BGMAudioComponent->bIsUISound = true;
    BGMAudioComponent->bStopWhenOwnerDestroyed = false;
    BGMAudioComponent->bAutoDestroy = false;
    

    // World破棄を監視
    FWorldDelegates::OnWorldCleanup.AddUObject(
        this, &UTD_GameInstance::HandleWorldCleanup);

    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

void UTD_GameInstance::Shutdown()
{
    FWorldDelegates::OnWorldCleanup.RemoveAll(this);
    Super::Shutdown();
}

void UTD_GameInstance::HandleWorldCleanup(
    UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
    if (BGMAudioComponent && BGMAudioComponent->IsRegistered())
    {
        UE_LOG(LogTemp, Warning, TEXT("Unregister AudioComponent (World Cleanup)"));
        BGMAudioComponent->UnregisterComponent();
    }
}

void UTD_GameInstance::PlayBGM(USoundBase* NewBGM, float FadeInTime)
{
    if (!NewBGM || !BGMAudioComponent)
        return;

    // World が変わっていたら再登録
    if (!BGMAudioComponent->IsRegistered())
    {
        BGMAudioComponent->RegisterComponentWithWorld(GetWorld());
        UE_LOG(LogTemp, Warning, TEXT("AudioComponent Registered With New World"));
    }

    // 同じBGMかつ再生中なら何もしない（途中再生維持）
    if (CurrentBGM == NewBGM && BGMAudioComponent->IsPlaying())
    {
        UE_LOG(LogTemp, Warning, TEXT("Same BGM - continue playing"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Play BGM: %s"), *NewBGM->GetName());

    CurrentBGM = NewBGM;

    BGMAudioComponent->SetSound(NewBGM);
   // UGameplayStatics::SpawnSound2D(this,CurrentBGM,1.0f,1.0f,0.0f,nullptr,true);
    BGMAudioComponent->FadeIn(FadeInTime, 1.0f);
}

void UTD_GameInstance::PlayBGMByType(
    EBGMType Type, float FadeInTime, bool bForceRestart)
{
    // 同じBGMかつ再スタート不要なら何もしない
    if (!bForceRestart &&
        BGMAudioComponent &&
        BGMAudioComponent->IsPlaying() &&
        CurrentBGMType == Type)
    {
        return;
    }

    USoundBase** Found = BGMTable.Find(Type);
    if (!Found || !(*Found))
    {
        return;
    }

    // 別BGMならフェードアウト
    if (BGMAudioComponent && BGMAudioComponent->IsPlaying())
    {
        BGMAudioComponent->FadeOut(0.5f, 0.0f);
    }

    CurrentBGMType = Type;

    PlayBGM(*Found, FadeInTime);
}


TSubclassOf<AWeaponBase> UTD_GameInstance::ResolveWeapon(EWeaponID WeaponID) const
{
    switch (WeaponID)
    {
    case EWeaponID::AssaultRifle:
        return AssaultRifleClass;

    case EWeaponID::Shotgun:
        return ShotgunClass;

    case EWeaponID::Pistol:
        return PistolClass;

    case EWeaponID::SMG:
        return SMGClass;

    case EWeaponID::HSG:
        return HSGClass;

    case EWeaponID::SR:
        return SRClass;

    case EWeaponID::SemiSG:
        return SemiSGClass;

    case EWeaponID::Uzi:
        return UziClass;

    case EWeaponID::GL:
        return GLClass;

    default:
        return nullptr;
    }
}

void UTD_GameInstance::SetStageCleared(int32 StageNum)
{
    if (StageNum > MaxClearedStage)
    {
        MaxClearedStage = StageNum;
    }
}