#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "MyHeroPlayer.h"
#include"AllyCharacter.h"
#include"DefenseBase.h"
#include"TD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtr.h"

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackRange = 150.0f;
    AttackCooldown = 2.0f;
    AttackDamage = 10.0f;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = 600.f;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


}

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<AMyHeroPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    CurrentHealth = MaxHealth;
}

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead || !PlayerCharacter) return;

    // 1. 攻撃対象を選ぶ
    AActor* Target = ChooseTarget();

    if (!Target) return;

    CurrentTarget = Target;

    // 2. 距離チェック
    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    //float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance <= AttackRange)
    {
        PerformAttack();
    }

    

}

AActor* AEnemyCharacterBase::ChooseTarget()
{
    // 1. 味方キャラクターをすべて取得
    TArray<AActor*> AllAllies;

    // プレイヤー
    if (PlayerCharacter)
    {
        AllAllies.Add(PlayerCharacter);
    }

    // 他の味方キャラクター
    TArray<AActor*> FoundAllies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);
    AllAllies.Append(FoundAllies);

    // 2. 最も近い味方を探索
    AActor* NearestAlly = nullptr;
    float NearestDistance = FLT_MAX;

    for (AActor* Ally : AllAllies)
    {
        if (!IsValid(Ally)) continue;

        float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
        if (Dist < NearestDistance)
        {
            NearestDistance = Dist;
            NearestAlly = Ally;
        }
    }

    if (NearestAlly)
    {
        return NearestAlly; // 最も近い味方をターゲットに
    }

    // 3. 味方がいなければ防衛対象（基地）を探す
    TArray<AActor*> FoundBases;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);

    if (FoundBases.Num() > 0)
    {
        return FoundBases[0];
    }



   // TArray<AActor*> FoundAllies;
   // UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);

   // if (FoundAllies.Num() > 0)
   // {
   //     return FoundAllies[0]; // 最初の味方を選択
   // }

   // // 優先度1：プレイヤーが近いならプレイヤーを狙う
   // if (IsValid(PlayerCharacter))
   // {
   //     const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
   //     if (DistanceToPlayer <= 800.f) // 例: 8m以内なら優先
   //     {
   //         return PlayerCharacter;
   //     }
   // }

   //  // 2. 味方キャラクターを検索して距離最短のものを選択
   //// TArray<AActor*> FoundAllies;
   // UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);

   // AActor* ClosestAlly = nullptr;
   // float ClosestDistance = FLT_MAX;

   // for (AActor* Ally : FoundAllies)
   // {
   //     // 自分自身は除外
   //     //if (Ally == this) continue;

   //     // 生存チェック
   //     AAllyCharacter* AllyChar = Cast<AAllyCharacter>(Ally);
   //     if (!AllyChar || IsValid(AllyChar)) continue;

   //     const float DistanceToAlly = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());

   //     // 800cm以内の味方だけ対象
   //     if (DistanceToAlly <= 2000.f && DistanceToAlly < ClosestDistance)
   //     {
   //         ClosestDistance = DistanceToAlly;
   //         ClosestAlly = Ally;
   //     }
   // }

   // if (ClosestAlly)
   // {
   //     return ClosestAlly;
   // }

   // // 優先度2：防衛対象（基地）を探す
   // TArray<AActor*> FoundBases;
   // UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);

   // if (FoundBases.Num() > 0)
   // {
   //     return FoundBases[0]; // 最初の基地をターゲットに
   // }

    return nullptr;
}

void AEnemyCharacterBase::PerformAttack()
{

    if (!bCanAttack || bIsDead || !CurrentTarget) return;

    bCanAttack = false;

    UE_LOG(LogTemp, Warning, TEXT("%s attacks %s!"), *GetName(), *CurrentTarget->GetName());

    UGameplayStatics::ApplyDamage(CurrentTarget, AttackDamage, GetController(), this, nullptr);

    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);


    //if (!bCanAttack || !PlayerCharacter || bIsDead) return;

    //AActor* Target = PlayerCharacter;
    //if (!Target)
    //{
    //    // 防衛対象を検索（プレイヤーがいない場合）
    //    TArray<AActor*> FoundBases;
    //    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
    //    if (FoundBases.Num() > 0)
    //    {
    //        Target = FoundBases[0];
    //    }
    //}

    //if (!Target) return;

    //bCanAttack = false;

    //UE_LOG(LogTemp, Warning, TEXT("Enemy attacks target: %s"), *Target->GetName());

    //UGameplayStatics::ApplyDamage(PlayerCharacter, AttackDamage, GetController(), this, nullptr);

    //GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
}

float AEnemyCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f;

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= ActualDamage;
    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
        *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }

    return ActualDamage;
}

void AEnemyCharacterBase::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died!"), *GetName());

    // TODO: アニメーションやエフェクトを追加したい場合はここで再生
    // PlayAnimMontage(DeathAnimMontage);

    // 一定時間後に消滅してもよい
    //SetLifeSpan(2.0f);

    // GameModeに通知
    ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
    {
        GM->OnEnemyDestroyed();
    }

    // すぐに消す場合は
     Destroy();
}
