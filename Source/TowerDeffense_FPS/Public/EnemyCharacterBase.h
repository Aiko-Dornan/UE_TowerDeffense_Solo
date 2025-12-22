#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"MyGrenadeProjectileActor.h"


#include "EnemyCharacterBase.generated.h"

class ADefenseBase;
class ADefenseStructure;
class AMyHeroPlayer;
class AAllyCharacter;
class ADroneCharacter;
class AEnemyAIController;

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

    void CheckIfStopped();

   

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

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<class AMyGrenadeProjectileActor> ProjectileClass;
    void AllRangeAttack();

    //void ApplyAreaDamage(float DamageAmount, float Radius);

    UFUNCTION(BlueprintCallable)
    void ApplyAreaDamage(float DamageAmount, float Radius);

    float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    void Die();

    void PlayNiagaraEffect();

    void StartMovingToTarget();
    void TryStartAI();

    float GetEffectiveAttackRange(AActor* Target) const;

    AActor* CheckBlockingStructure(AActor* MainTarget);

    FORCEINLINE AActor* GetCurrentTarget() const { return CurrentTarget; }
    FORCEINLINE void SetCurrentTarget(AActor* NewTarget) { CurrentTarget = NewTarget; }

    //void UpdateTarget();

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

    UPROPERTY()
    AActor* CurrentTarget = nullptr;
private:
   
   
    UPROPERTY()
    AActor* PreviousTarget = nullptr; // 建物破壊後に戻る用

    bool bIsDead = false;
    bool bCanAttack = true;
    bool bNotifiedSpawner = false;

    float AttackRange;
    float AttackCooldown;
    

    
    float CurrentHealth;

    float LastBarrierCheckTime = 0.f;
    float BarrierCheckCooldown = 1.0f; // 1秒に1回だけチェック

   

    bool bHasLoggedStuck = false;

    bool bUseDirectMove = false;

    FTimerHandle AttackCooldownTimerHandle;

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
};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//class AEnemyAIController;
//class AMyHeroPlayer;
//class ADefenseBase;
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//    virtual void Tick(float DeltaTime) override;
//    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
//        AController* EventInstigator, AActor* DamageCauser) override;
//
//    void PerformAttack();
//    float GetEffectiveAttackRange(AActor* Target) const;
//
//    // --- Blueprintでオーバーライド可能なターゲット選定 ---
//    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Targeting")
//    AActor* ChooseTargetBP();
//    virtual AActor* ChooseTargetBP_Implementation();
//
//    AActor* ChooseTarget();  // ターゲット選択関数
//
//protected:
//    virtual void BeginPlay() override;
//    
//    void ResetAttack();
//    void Die();  // 死亡処理
//
//public:
//    // 攻撃設定
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackRange;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackCooldown;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackDamage = 10.0f;
//
//    // === HP関連 ===
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
//    float MaxHealth = 100.0f;
//
//    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
//    float CurrentHealth;
//
//    //1回攻撃したら自滅するフラグ
//    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
//    bool KillMeFlag;
//
//    // --- ターゲット優先度を変数化 ---
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
//    float PlayerPriority = 100.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
//    float AllyPriority = 80.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
//    float BasePriority = 50.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Targeting")
//    float StructurePriority = 60.f;
//
//    USTRUCT(BlueprintType)
//        struct FTargetCandidate
//    {
//        GENERATED_BODY()
//
//        UPROPERTY(BlueprintReadWrite)
//        AActor* Actor = nullptr;
//
//        UPROPERTY(BlueprintReadWrite)
//        float Score = 0.f;
//    };
//
//
//private:
//    bool bCanAttack = true;
//    bool bIsDead = false;
//    bool bNotifiedSpawner = false;
//
//    FTimerHandle AttackCooldownTimerHandle;
//    AMyHeroPlayer* PlayerCharacter;
//    ADefenseBase* BaseStructure;
//
//    AActor* CurrentTarget;  // 現在狙っている対象
//};
