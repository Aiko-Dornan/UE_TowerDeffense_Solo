#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include"AmmoDisplayWidget.h"
#include"MyHeroPlayer.h"

#include "AllyCharacter.generated.h"

class AEnemyCharacterBase;
class AWeaponBase;
class AAllyAIController;

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
    void SetTargetEnemy(AActor* NewTarget);

    void MoveToInitialPosition();

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

private:
    /** 射撃タイマー */
    FTimerHandle FireTimerHandle;

    /** 敵更新タイマー */
    FTimerHandle TargetUpdateTimer;
};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "AAllyAIController.h"
//#include "WeaponBase.h"
//#include "EnemyCharacterBase.h"
//#include "AllyCharacter.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    FVector InitialPosition; // 初期位置
//    // 敵検知範囲
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float EnemyDetectRange = 1000.f; // この範囲内に敵がいる場合のみ戦闘
//
//public:
//    virtual void Tick(float DeltaTime) override;
//
//    // 武器
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY()
//    AWeaponBase* EquippedWeapon;
//
//    // 敵ターゲット
//    UPROPERTY()
//    AEnemyCharacterBase* TargetEnemy;
//
//    // 健康
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float MaxHealth = 100.f;
//
//    UPROPERTY(VisibleAnywhere)
//    float CurrentHealth;
//
//    // 射撃
//    UPROPERTY(EditAnywhere)
//    float FireRange = 1500.f;
//
//    UPROPERTY(EditAnywhere)
//    float FireInterval = 0.5f;
//
//    FTimerHandle FireTimerHandle;
//    FTimerHandle TargetUpdateTimer;
//
//    // 移動
//    UPROPERTY(EditAnywhere)
//    float MaintainDistance = 500.f;
//
//    UPROPERTY(EditAnywhere)
//    float AcceptableRadius = 10.f;
//
//    // 視界角度（正面方向からの最大角度）
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float SightAngle = 60.f;
//
//    // 敵検知最大距離
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float SightDistance = 1000.f;
//
//    // 関数
//    void FaceTarget(AActor* Target);
//    void FindNearestEnemy();
//    void HandleFire();
//    void StopMovement();
//    void SetTargetEnemy(AActor* NewTarget);
//    float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
//    void Die();
//    bool IsEnemyVisible(AEnemyCharacterBase* Enemy);
//};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "WeaponBase.h"
//#include "EnemyCharacterBase.h"
//#include "AAllyAIController.h" // ←専用AIControllerをインクルード
//#include "AllyCharacter.generated.h" // ←必ず最後に置く
//
//class AWeaponBase;
//class AEnemyCharacterBase;
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaTime) override;
//
//public:
//    /** AI関連 */
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MaintainDistance = 500.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AcceptableRadius = 50.f;
//
//    /** 射撃関連 */
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireRange = 800.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    /** HP関連 */
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
//    float MaxHealth = 100.f;
//
//    float CurrentHealth;
//
//    /** 武器 */
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    AWeaponBase* EquippedWeapon;
//
//    /** 敵ターゲット */
//    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
//    AEnemyCharacterBase* TargetEnemy;
//
//    FTimerHandle TargetUpdateTimer;
//    FTimerHandle FireTimerHandle;
//
//    /** 敵探索 */
//    void FindNearestEnemy();
//
//    /** 射撃 */
//    void HandleFire();
//
//    /** 後退移動 */
//    void MoveToSafeDistance(AActor* Target, float DesiredDistance);
//
//    /** 敵方向を向く */
//    void FaceTarget(AActor* Target);
//
//    void StopMovement();
//
//    void SetTargetEnemy(AActor* NewTarget);
//
//    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
//        class AController* EventInstigator, AActor* DamageCauser) override;
//
//    void Die();
//};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "AllyCharacter.generated.h"
//
//class AWeaponBase;
//class AEnemyCharacterBase;
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//
//public:
//    virtual void Tick(float DeltaTime) override;
//
//    UPROPERTY(EditAnywhere, Category = "Combat")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireRange = 1000.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
//    float MaintainDistance = 500.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
//    float AcceptableRadius = 50.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
//    float MaxHealth = 100.f;
//
//    float CurrentHealth;
//
//    void SetTargetEnemy(AActor* NewTarget);
//
//protected:
//    UPROPERTY()
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY()
//    AEnemyCharacterBase* TargetEnemy;
//
//    FTimerHandle FireTimerHandle;
//    FTimerHandle TargetUpdateTimer;
//
//    void FindNearestEnemy();
//    void FaceTarget(AActor* Target);
//    void HandleFire();
//    void StopMovement();
//
//    void MoveToSafeDistance(AActor* Target, float DesiredDistance);
//
//    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
//        AController* EventInstigator, AActor* DamageCauser) override;
//
//    void Die();
//};




//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "WeaponBase.h"
//#include "AllyCharacter.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaTime) override;
//
//    // ===== AI設定 =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MaintainDistance = 600.0f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AcceptableRadius = 50.0f;
//
//    UPROPERTY(BlueprintReadWrite, Category = "AI")
//    AActor* TargetEnemy;
//
//    // ===== 武器 =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireRange = 3000.0f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    FTimerHandle FireTimerHandle;
//
//    // 敵再探索用タイマー（これを追加！）
//    FTimerHandle TargetUpdateTimer;
//
//    // ===== 内部関数 =====
//    void HandleFire();
//    void MoveTowards(AActor* Target, float DesiredDistance);
//    void MoveAwayFrom(AActor* Target, float DesiredDistance);
//    void StopMovement();
//    void FaceTarget(AActor* Target);
//    void FindNearestEnemy(); // Tickごとに呼ばれる敵探索
//
//public:
//    UFUNCTION(BlueprintCallable, Category = "AI")
//    void SetTargetEnemy(AActor* NewTarget);
//};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "WeaponBase.h"
//#include "AllyCharacter.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaTime) override;
//
//    // ===== AI追従 =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MaintainDistance = 600.0f; // 敵と保ちたい距離
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AcceptableRadius = 50.0f; // 到達誤差
//
//    UPROPERTY(BlueprintReadWrite, Category = "AI")
//    AActor* TargetEnemy; // 追尾する敵
//
//    // ===== 武器 =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireRange = 3000.0f;
//
//    // 射撃間隔
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    FTimerHandle FireTimerHandle;
//
//    void HandleFire();
//
//    void MoveTowards(AActor* Target, float DesiredDistance);
//    void MoveAwayFrom(AActor* Target, float DesiredDistance);
//    void StopMovement();
//
//public:
//    UFUNCTION(BlueprintCallable, Category = "AI")
//    void SetTargetEnemy(AActor* NewTarget);
//
//};
//
//


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "WeaponBase.h"
//#include "AllyCharacter.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    
//
//    // ==== 武器関連 ====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    FName WeaponAttachSocketName = "WeaponSocket";
//
//    void EquipWeapon(AWeaponBase* NewWeapon);
//
//    // ==== 攻撃関連 ====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    FTimerHandle FireTimerHandle;
//
//    void FireAtTarget();
//
//    AActor* FindNearestEnemy();
//
//public:
//    virtual void Tick(float DeltaTime) override;
//};
