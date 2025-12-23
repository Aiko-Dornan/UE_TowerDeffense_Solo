#include "TD_GameInstance.h"
#include "WeaponBase.h"

void UTD_GameInstance::Init()
{
    Super::Init();

    BGMAudioComponent = NewObject<UAudioComponent>(this);
    BGMAudioComponent->bAutoActivate = false;
    BGMAudioComponent->bIsUISound = true;
    BGMAudioComponent->bStopWhenOwnerDestroyed = false;

    // š World”jŠü‚ðŠÄŽ‹
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
        BGMAudioComponent->Stop();
        BGMAudioComponent->UnregisterComponent();
    }
}

void UTD_GameInstance::PlayBGM(USoundBase* NewBGM, float FadeInTime)
{
    if (!NewBGM || !BGMAudioComponent)
        return;

    // š ‚±‚±‚ªƒ|ƒCƒ“ƒg
    if (CurrentBGM == NewBGM && BGMAudioComponent->IsPlaying())
    {
        UE_LOG(LogTemp, Warning, TEXT("Same BGM - continue playing"));
        return;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Different BGM,%s"),*NewBGM->GetName());
    }

    CurrentBGM = NewBGM;

    // š World‚ª•Ï‚í‚Á‚Ä‚¢‚½‚çÄ“o˜^
    if (!BGMAudioComponent->IsRegistered())
    {
        BGMAudioComponent->RegisterComponentWithWorld(GetWorld());
        UE_LOG(LogTemp, Warning, TEXT("AudioComponent Registered With New World"));
    }

    BGMAudioComponent->SetSound(NewBGM);
    BGMAudioComponent->FadeIn(FadeInTime, 1.0f);
}

void UTD_GameInstance::PlayBGMByType(EBGMType Type, float FadeInTime)
{
    if (CurrentBGMType == Type && BGMAudioComponent->IsPlaying())
        return;

    USoundBase** Found = BGMTable.Find(Type);
    if (!Found) return;

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

    default:
        return nullptr;
    }
}
