#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include"AmmoDisplayWidget.h"
#include"MyHeroPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AllyCharacter.generated.h"

class AEnemyCharacterBase;
class AWeaponBase;
class AAllyAIController;

UENUM(BlueprintType)
enum class EAllyAnimType : uint8
{
    Idle       /* UMETA(DisplayName = "Idle")*/,
    Move_Front,
    Move_Back,
    Move_Left,
    Move_Right,
    Attack      /*UMETA(DisplayName = "Attack")*/,
    RangeAttack /*UMETA(DisplayName = "RangeAttack")*/,
    Dead        /*UMETA(DisplayName = "Dead")*/,
    Damage,
};

UCLASS()
class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAllyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY()
    class AMyHeroPlayer* MHP;

public:
    /** ダメージ処理 */
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    /** 射撃処理 */
    void HandleFire();

    /** 敵を探す */
    void FindNearestEnemy();

    /** 敵の方向を向く */
    void FaceTarget(AActor* Target);

    /** 初期位置に戻る */
    void MoveBackToInitialPosition();

    /** 敵に近づく */
    void MoveTowardsEnemy(AActor* Target);

    /** 敵から離れる（後退） */
    void MoveAwayFromEnemy(AActor* Target);

    /** 移動停止 */
    void StopMovement();

    /** 死亡処理 */
    void Die();

    /** 敵設定 */
    //void SetTargetEnemy(AActor* NewTarget);

    //void MoveToInitialPosition();

    UFUNCTION()//武器ドロップ
        void DropCurrentWeapon();

    /** 味方の現在 HP */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentHealth = 100.f;

    /** 最大 HP */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    /** 射撃可能距離 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 3000.f;

    /** 射撃間隔 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 0.5f;

    /** 敵を検知する範囲 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float EnemyDetectRange = 2000.f;

    /** 距離を維持するための理想距離 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float MaintainDistance = 800.f;

    /** 許容半径（停止判定） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AcceptableRadius = 100.f;

    /** 使用する武器のクラス */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TSubclassOf<AWeaponBase> WeaponClass;

    /** 装備中の武器 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    AWeaponBase* EquippedWeapon;

    /** 敵ターゲット */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    AEnemyCharacterBase* TargetEnemy;

    /** ターゲットを保持する最大時間 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float TargetLockTime = 3.0f;

    /** ターゲット解除距離 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float TargetLoseDistance = 2500.f;

    /** 現在のターゲットをロックした時間 */
    float TargetLockedElapsed = 0.f;

    /** 初期位置 */
    FVector InitialPosition;

    enum EAllyState :uint8
    {
        Idle,
        Move,
        Attack,
        RangeATtack,
        Dead,
        Damage,

    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* IdleAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* MoveFrontAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* MoveBackAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* MoveLeftAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* MoveRightAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* AttackAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* RangeAttackAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* DeadAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimationAsset* DamageAnim;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    EAllyAnimType EnemyState = EAllyAnimType::Idle;

    EAllyAnimType GetMoveAnimByDirection() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float GetawayTime = 4.0f;//逃走する時間

   // UFUNCTION(BlueprintCallable, Category = "Enemy|Animation")
    // void SetEnemyState(EEnemyState NewState);

    void PlayAnimation(EAllyAnimType NewType, bool bLoop);

    UAnimationAsset* GetAnimByType(EAllyAnimType Type) const;

    // 再生中ロック
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bIsAnimationLocked = false;

    // 現在再生中のアニメ種別
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    EAllyAnimType CurrentAnimType;

    // ロック対象のアニメか？
    bool IsLockedAnim(EAllyAnimType Type) const;

    void LockRelease();
    FTimerHandle LockReleaseHandle;

    void MoveORIdle();

    void UpdateMoveAnimation();

private:
    /** 射撃タイマー */
    FTimerHandle FireTimerHandle;

    /** 敵更新タイマー */
    FTimerHandle TargetUpdateTimer;

    bool bIsDead = false;

    float AnimChangeCooldown = 0.15f;
    float AnimChangeElapsed = 0.f;
    float ReGetawayTime = 4.0f;
    bool bIsGetaway = false;
    FTimerHandle GetawayTimerHandle;

    void StartGetaway(float Duration);
    void EndGetaway();
    bool bIsGetawayMoving = false;
    bool bReturningHome = false;

};


