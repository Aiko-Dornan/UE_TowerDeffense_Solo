#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"MyGrenadeProjectileActor.h"
#include"Components/BoxComponent.h"

#include "EnemyCharacterBase.generated.h"

class ADefenseBase;
class ADefenseStructure;
class AMyHeroPlayer;
class AAllyCharacter;
class ADroneCharacter;
class AEnemyAIController;

UENUM(BlueprintType)
enum class /*EEnemyState*/EEnemyAnimType : uint8
{
    Idle       /* UMETA(DisplayName = "Idle")*/,
    Move       /* UMETA(DisplayName = "Move")*/,
    Attack      /*UMETA(DisplayName = "Attack")*/,
    RangeAttack /*UMETA(DisplayName = "RangeAttack")*/,
    Dead        /*UMETA(DisplayName = "Dead")*/,
    Damage,
};



USTRUCT(BlueprintType)
struct FTargetCandidate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    AActor* Actor = nullptr;

    UPROPERTY(BlueprintReadWrite)
    float Score = 0.f;
};

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

protected:
    virtual void BeginPlay() override;

    float DefaultConsiderRange = 4000.f;
    float ExtendedConsiderRange = 8000.f; // 停止中の探索距離
    float StopCheckInterval = 1.0f;        // 停止チェック間隔
    float StopThresholdSpeed = 10.f;       // 速度がこの以下なら停止中
    float StopDurationToExtend = 2.0f;     // 何秒停止したら距離を広げるか

    float TimeSinceLastMove = 0.f;
    bool bIsRecognitionExtended = false;

    bool blocking_flag{ false };

    enum EEnemyState :uint8
    {
        Idle   ,
        Move      ,
        Attack    ,
        RangeATtack ,
        Dead,
        Damage,

    };

   

public:
    virtual void Tick(float DeltaTime) override;

   

    // --- オーバーラップイベント ---
    UFUNCTION()
    void OnEnemyBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    // --- 建物破壊通知を受ける ---
    UFUNCTION()
    void OnTargetDestroyed(AActor* DestroyedActor);

    // --- ターゲット選定 ---
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AActor* ChooseTargetBP();
    virtual AActor* ChooseTargetBP_Implementation();

    AActor* ChooseTarget_Default();

    void UpdateTarget();

    void PerformAttack();
    void ResetAttack();
    FTimerHandle AttackCooldownTimerHandle;
    float AttackCooldown;
    bool bCanAttack = true;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AMyGrenadeProjectileActor> ProjectileClass;
    void AllRangeAttack();

    //void ApplyAreaDamage(float DamageAmount, float Radius);

    UFUNCTION(BlueprintCallable)
    void ApplyAreaDamage(float DamageAmount, float Radius);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UBoxComponent* AttackCollision;

    UPROPERTY()
    TSet<AActor*> HitActors; // 多段ヒット防止

    UFUNCTION()
    void OnAttackOverlap(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);

    float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    void Die();

    void PlayNiagaraEffect();

    void StartMovingToTarget();
    void TryStartAI();

    //Animation

    USkeletalMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
    UAnimationAsset* IdleAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
    UAnimationAsset* MoveAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
    UAnimationAsset* AttackAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
    UAnimationAsset* RangeAttackAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
    UAnimationAsset* DeadAnim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Animation")
    UAnimationAsset* DamageAnim;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Animation")
    EEnemyAnimType EnemyState = EEnemyAnimType::Idle;

    UFUNCTION(BlueprintCallable, Category = "Enemy|Animation")
   // void SetEnemyState(EEnemyState NewState);

    void PlayAnimation(EEnemyAnimType NewType, bool bLoop);

    UAnimationAsset* GetAnimByType(EEnemyAnimType Type) const;

    // 再生中ロック
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Animation")
    bool bIsAnimationLocked = false;

    // 現在再生中のアニメ種別
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Animation")
    EEnemyAnimType CurrentAnimType;

    // ロック対象のアニメか？
    bool IsLockedAnim(EEnemyAnimType Type) const;

    void LockRelease();
    FTimerHandle LockReleaseHandle;

    void MoveORIdle();

    UFUNCTION()
    void OnAttackHit();


    float GetEffectiveAttackRange(AActor* Target) const;

    AActor* CheckBlockingStructure(AActor* MainTarget)const;

    FORCEINLINE AActor* GetCurrentTarget() const { return CurrentTarget; }
    FORCEINLINE void SetCurrentTarget(AActor* NewTarget) { CurrentTarget = NewTarget; }

    bool CanReachTarget(AActor* Target) const;
    bool ShouldSwitchToBlocking(AActor* Target, AActor*& OutBlocking) const;

    bool ShouldAttackBlockingStructure(
        AActor* Target,
        AActor*& OutBlocking
    ) const;

    //void UpdateTarget();

    // どこまでの迂回を許すか（1.5 = 50%増しまでOK）
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI|Target")
    float AllowDetourRatio = 1.5f;


        // --- ターゲット優先度を変数化 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
    float PlayerPriority = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
    float AllyPriority = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
    float BasePriority = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
    float StructurePriority = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
    float DronePriority = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|AttackRange")
    float BaseAttackRange = 600.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|AttackRange")
    float PlayerAllyAttackRange = 200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|AttackRange")
    float BarricadeAttackRange = 200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|AttackRange")
    float DroneAttackRange = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|AllRangeFlag")
    bool RangeAttack = false;
   
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float AttackDamage=10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float MaxHealth=100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float MoveEnemySpeed = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    bool KillMeFlag = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float AmountAreaAtacck=500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UNiagaraSystem* NiagaraEffect;

    UPROPERTY(EditAnywhere,BlueprintReadOnly)
    bool law_speed_flag = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    AActor* CurrentTarget = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly )
    bool LockTarget = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    AActor* BlockTarget = nullptr;
private:
   
   
    UPROPERTY()
    AActor* PreviousTarget = nullptr; // 建物破壊後に戻る用

    bool bIsDead = false;
   
    bool bNotifiedSpawner = false;

    float AttackRange;
    
    

    
    float CurrentHealth;

    float LastBarrierCheckTime = 0.f;
    float BarrierCheckCooldown = 1.0f; // 1秒に1回だけチェック

   

    bool bHasLoggedStuck = false;

    bool bUseDirectMove = false;

    
    UPROPERTY()
    AMyHeroPlayer* PlayerCharacter = nullptr;

    UPROPERTY()
    ADefenseBase* BaseStructure = nullptr;

    // AEnemyCharacterBase.h
private:
    FTimerHandle TargetUpdateTimerHandle;
    float TargetUpdateInterval = 0.5f; // 0.5秒ごとにターゲット更新

    FTimerHandle StopCheckTimerHandle;

    FTimerHandle TryStartTimerHandle;

    float LastMoveCommandTime = 0.0f;

    float GetNavPathLengthToTarget(AActor* Target) const;

    // ===== 追加：NavMesh 到達判定 =====
    //bool CanReachTargetByNav(AActor* Target) const;

    

};

