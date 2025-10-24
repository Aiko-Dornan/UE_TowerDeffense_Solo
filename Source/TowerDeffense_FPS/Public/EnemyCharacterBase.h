#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacterBase.generated.h"

class AEnemyAIController;

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

    virtual void Tick(float DeltaTime) override;

    void PerformAttack();

protected:
    virtual void BeginPlay() override;

public:
    // 攻撃範囲
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange;

    // 攻撃クールダウン
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackCooldown;

private:
    bool bCanAttack = true;
    FTimerHandle AttackCooldownTimerHandle;

    void ResetAttack();
};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//protected:
//    virtual void BeginPlay() override;
//
//public:
//    virtual void Tick(float DeltaTime) override;
//
//    // 攻撃処理（AIController から呼ぶため public に）
//    void PerformAttack();
//
//    // 攻撃範囲
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackRange;
//
//    // 攻撃クールダウン（秒）
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackCooldown;
//
//    // 移動速度
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MoveSpeed;
//
//private:
//    bool bCanAttack;
//    FTimerHandle AttackCooldownTimerHandle;
//
//    void ResetAttack();
//};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//protected:
//    virtual void BeginPlay() override;
//
//    UPROPERTY()
//    APawn* PlayerPawn;
//
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackRange = 150.0f;
//
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackCooldown = 2.0f;
//
//    bool bCanAttack = true;
//
//    FTimerHandle AttackCooldownTimerHandle;
//
//    void PerformAttack();
//    void ResetAttack();
//
//
//
//public:
//    virtual void Tick(float DeltaTime) override;
//};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//protected:
//    virtual void BeginPlay() override;
//
//    // ターゲット(プレイヤー)
//    UPROPERTY()
//    APawn* PlayerPawn;
//
//    // 攻撃範囲
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackRange;
//
//    // 攻撃クールダウン（秒）
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackCooldown;
//
//    // 移動速度
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float MoveSpeed;
//
//    bool bCanAttack;
//
//    FTimerHandle AttackCooldownTimerHandle;
//
//    // 攻撃処理
//    void PerformAttack();
//
//    void ResetAttack();
//
//    // 移動処理
//    void MoveToPlayer();
//
//public:
//    virtual void Tick(float DeltaTime) override;
//};
