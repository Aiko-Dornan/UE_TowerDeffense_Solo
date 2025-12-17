#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    IT_Consumable UMETA(DisplayName = "Consumable"), // 回復アイテムなど
    IT_Weapon     UMETA(DisplayName = "Weapon")     // 武器（インベントリには入れない）
};

UCLASS()
class TOWERDEFFENSE_FPS_API AItemBase : public AActor
{
    GENERATED_BODY()

public:
    AItemBase();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    // アイテムタイプ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType = EItemType::IT_Consumable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* ItemIcon; // ← アイコン追加

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    bool bIsConsumable = true; // 消耗品かどうか

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FString ItemName;

    // 武器専用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsWeapon = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 SavedAmmo = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 SavedStockAmmo = 0;

    // 消耗品用（回復量）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float HealAmount = 0.f;

    UFUNCTION(BlueprintCallable, Category = "Item")
    void OnPickedUp(class AMyHeroPlayer* Player);

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};
