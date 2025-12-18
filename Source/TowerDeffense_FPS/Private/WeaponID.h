#pragma once
#include "WeaponID.generated.h"

UENUM(BlueprintType)
enum class EWeaponID : uint8
{
    None UMETA(DisplayName = "None"),

    AssaultRifle UMETA(DisplayName = "Assault Rifle"),
    Shotgun      UMETA(DisplayName = "Shotgun"),
    Pistol       UMETA(DisplayName = "Pistol"),
    SMG          UMETA(DisplayName = "SMG")
};
