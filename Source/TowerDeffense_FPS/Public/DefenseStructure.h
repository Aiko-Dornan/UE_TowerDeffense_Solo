#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefenseStructure.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ADefenseStructure : public AActor
{
    GENERATED_BODY()

public:
    ADefenseStructure();

protected:
    virtual void BeginPlay() override;

public:
    // ダメージ処理
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // 壊れる処理
    void OnDestroyedByEnemy();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    float MaxHealth = 200.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defense")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    UStaticMeshComponent* MeshComp;

    // 壊れたときのエフェクト
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    UParticleSystem* DestroyEffect;

    // 壊れたときのサウンド
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defense")
    USoundBase* DestroySound;

    // 死亡フラグ
    bool bIsDestroyed = false;
};
