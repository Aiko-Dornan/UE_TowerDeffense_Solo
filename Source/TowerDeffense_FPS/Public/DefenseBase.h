#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"   // ← これ大事！AActorを使うため
#include "Delegates/DelegateCombinations.h"
#include "Components/WidgetComponent.h"
#include"Components/BoxComponent.h"
#include "DefenseBase.generated.h" // ← 必ず最後に書く

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBaseDestroyed);

UCLASS()
class TOWERDEFFENSE_FPS_API ADefenseBase : public AActor   // ← UObjectではなくAActor！
{
    GENERATED_BODY()

public:
    ADefenseBase();

    virtual void Tick(float DeltaTime) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;  // ← AActorが持っている関数（UObjectにはない）

    void OnDestroyedBase();

  

public:
  /*  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UBoxComponent* HitCollision;*/

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
    float MaxHealth = 500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Mesh")
    UStaticMeshComponent* BaseMesh;

    //// HP表示用
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    //UWidgetComponent* HPWidgetComponent;

    float GetMaxHP() const;
    float GetCurrentHP() const;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnBaseDestroyed OnBaseDestroyed;
};
