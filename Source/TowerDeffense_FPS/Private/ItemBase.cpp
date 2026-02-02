#include "ItemBase.h"
#include "MyHeroPlayer.h"
#include "WeaponBase.h"
#include "Components/StaticMeshComponent.h"

AItemBase::AItemBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->BodyInstance.bLockXRotation = true;
    Mesh->BodyInstance.bLockYRotation = true;
    Mesh->BodyInstance.bLockZRotation = true;
}

void AItemBase::BeginPlay()
{
    Super::BeginPlay();
}

void AItemBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItemBase::OnPickedUp(AMyHeroPlayer* Player)
{
    if (!Player) return;

    if (ItemType == EItemType::IT_Weapon)
    {
        // •Ší‚È‚ç]—ˆ’Ê‚è‘•”õ
        if (!Player->MainSubFlag)
            Player->GunComponent = WeaponClass;
        else
            Player->GunComponentSub = WeaponClass;

        Player->EquipWeapon(WeaponClass);

        if (Player->CurrentWeapon)
        {
            Player->CurrentWeapon->Ammo = SavedAmmo;
            Player->CurrentWeapon->StockAmmo = SavedStockAmmo;
        }

       // Player->VaultAmmoNum();
        UE_LOG(LogTemp, Warning, TEXT("PickUpWeapon."));

        Destroy();
    }
}
