#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacterBase.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

protected:
    virtual void BeginPlay() override;

    // ターゲット(プレイヤー)
    UPROPERTY()
    APawn* PlayerPawn;

    // 攻撃範囲
    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange;

    // 攻撃クールダウン（秒）
    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackCooldown;

    // 移動速度
    UPROPERTY(EditAnywhere, Category = "AI")
    float MoveSpeed;

    bool bCanAttack;

    FTimerHandle AttackCooldownTimerHandle;

    // 攻撃処理
    void PerformAttack();

    void ResetAttack();

    // 移動処理
    void MoveToPlayer();

public:
    virtual void Tick(float DeltaTime) override;
};
