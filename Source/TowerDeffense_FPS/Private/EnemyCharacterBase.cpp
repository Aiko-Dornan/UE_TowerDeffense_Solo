#include "EnemyCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include"EnemyAIController.h"

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackRange = 150.0f;    // 近接攻撃レンジ
    AttackCooldown = 2.0f;   // 攻撃クールダウン秒数
    MoveSpeed = 5.0f;       //移動速度
    bCanAttack = true;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    GetCharacterMovement()->MaxWalkSpeed = 600.f;  // 好きな速度に設定
}

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);


}

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PlayerPawn) return;

    FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
    float DistanceToPlayer = Direction.Size();

    if (DistanceToPlayer > AttackRange)
    {
        // プレイヤーに向かって移動
        MoveToPlayer();
    }
    else
    {
        // 攻撃範囲内なら攻撃
        if (bCanAttack)
        {
            PerformAttack();
        }
        // 停止
        GetCharacterMovement()->StopMovementImmediately();
    }
}

void AEnemyCharacterBase::MoveToPlayer()
{
    if (!PlayerPawn) return;

    FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

    AddMovementInput(Direction, 1.0f);

    UE_LOG(LogTemp, Warning, TEXT("Movement!!"));
    UE_LOG(LogTemp, Warning, TEXT("%s"),*PlayerPawn->GetName());
}

void AEnemyCharacterBase::PerformAttack()
{
    bCanAttack = false;

    // 攻撃処理例：ログ出力
    UE_LOG(LogTemp, Warning, TEXT("Enemy attacks player!"));

    // 攻撃クールダウンタイマーをセット
    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
}
