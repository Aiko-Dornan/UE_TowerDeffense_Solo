#include "TD_GameInstance.h"
#include "WeaponBase.h"

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
