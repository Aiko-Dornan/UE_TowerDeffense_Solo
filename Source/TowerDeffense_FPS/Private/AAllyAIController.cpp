#include "AAllyAIController.h"
#include "AllyCharacter.h"

void AAllyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // 必要なら初期行動やブラックボード初期化などをここに追加
    if (AAllyCharacter* Ally = Cast<AAllyCharacter>(InPawn))
    {
        // 例: 初期ターゲット探索
        Ally->FindNearestEnemy();
    }
}


//#include "AAllyAIController.h"
//#include "AllyCharacter.h"
//#include "EnemyCharacterBase.h"
//#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "EngineUtils.h"
//#include "NavigationSystem.h"
//
//AAllyAIController::AAllyAIController()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AAllyAIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//
//    ControlledAlly = Cast<AAllyCharacter>(InPawn);
//
//    if (ControlledAlly)
//    {
//        UE_LOG(LogTemp, Warning, TEXT(" AllyAIController Possessed: %s"), *ControlledAlly->GetName());
//        GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyAIController::UpdateTargetEnemy, 0.5f, true);
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT(" AllyAIController failed to possess pawn!"));
//    }
//
//    if (ControlledAlly)
//    {
//        UE_LOG(LogTemp, Log, TEXT("AllyAIController Possessed: %s"), *ControlledAlly->GetName());
//        // ここで敵探索タイマー開始
//        GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyAIController::UpdateTargetEnemy, 0.5f, true);
//    }
//}
//
//void AAllyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    if (!ControlledAlly || !TargetEnemy) return;
//
//    MaintainCombatDistance(DeltaSeconds);
//    FaceEnemy(DeltaSeconds);
//}
//
//void AAllyAIController::UpdateTargetEnemy()
//{
//    if (!ControlledAlly) return;
//
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//
//        float Distance = FVector::Dist(ControlledAlly->GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//    ControlledAlly->SetTargetEnemy(TargetEnemy);
//}
//
//void AAllyAIController::MaintainCombatDistance(float DeltaSeconds)
//{
//   
//
//    if (!TargetEnemy || !ControlledAlly) return;
//
//    const float Distance = FVector::Dist(TargetEnemy->GetActorLocation(), ControlledAlly->GetActorLocation());
//    const float MaintainDistance = ControlledAlly->MaintainDistance;
//    const float TooCloseDistance = ControlledAlly->TooCloseDistance;
//    const float AcceptableRadius = ControlledAlly->AcceptableRadius;
//
//    UE_LOG(LogTemp, Log, TEXT("AI tick: %s, Target: %s, Distance: %.1f"),
//        *ControlledAlly->GetName(),
//        *TargetEnemy->GetName(),
//        Distance);
//
//    // --- NavMeshが有効か確認 ---
//    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
//    if (!NavSys) return;
//
//    FVector Destination;
//
//    // --- 近すぎたら後退 ---
//    if (Distance < TooCloseDistance)
//    {
//        FVector AwayDir = (ControlledAlly->GetActorLocation() - TargetEnemy->GetActorLocation()).GetSafeNormal();
//        Destination = ControlledAlly->GetActorLocation() + AwayDir * 300.f;
//        MoveToLocation(Destination, AcceptableRadius, false);
//        return;
//    }
//
//    // --- 離れすぎていれば前進 ---
//    if (FMath::Abs(Distance - MaintainDistance) > AcceptableRadius)
//    {
//        FVector ToTarget = (TargetEnemy->GetActorLocation() - ControlledAlly->GetActorLocation()).GetSafeNormal();
//        Destination = TargetEnemy->GetActorLocation() - ToTarget * MaintainDistance;
//        MoveToLocation(Destination, AcceptableRadius, false);
//    }
//    else
//    {
//        StopMovement();
//    }
//}
//
//void AAllyAIController::FaceEnemy(float DeltaSeconds)
//{
//    if (!TargetEnemy || !ControlledAlly) return;
//
//    FVector ToEnemy = TargetEnemy->GetActorLocation() - ControlledAlly->GetActorLocation();
//    ToEnemy.Z = 0.f;
//
//    FRotator LookRot = FRotationMatrix::MakeFromX(ToEnemy).Rotator();
//    ControlledAlly->SetActorRotation(
//        FMath::RInterpTo(ControlledAlly->GetActorRotation(), LookRot, DeltaSeconds, 5.f)
//    );
//}


//#include "AAllyAIController.h"
//#include "AllyCharacter.h"
//#include "EnemyCharacterBase.h"
//#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "EngineUtils.h"
//
//AAllyAIController::AAllyAIController()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AAllyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//
//    ControlledAlly = Cast<AAllyCharacter>(GetPawn());
//    if (!ControlledAlly) return;
//
//    // 敵の更新を定期実行
//    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyAIController::UpdateTargetEnemy, 0.5f, true);
//}
//
//void AAllyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    if (!ControlledAlly || !TargetEnemy) return;
//
//    MaintainCombatDistance(DeltaSeconds);
//    FaceEnemy(DeltaSeconds);
//}
//
//void AAllyAIController::UpdateTargetEnemy()
//{
//    if (!ControlledAlly) return;
//
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//
//        float Distance = FVector::Dist(ControlledAlly->GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//    ControlledAlly->SetTargetEnemy(TargetEnemy);
//}
//
//void AAllyAIController::MaintainCombatDistance(float DeltaSeconds)
//{
//    if (!TargetEnemy || !ControlledAlly) return;
//
//    float Distance = FVector::Dist(TargetEnemy->GetActorLocation(), ControlledAlly->GetActorLocation());
//
//    const float MaintainDistance = ControlledAlly->MaintainDistance;
//    const float TooCloseDistance = ControlledAlly->TooCloseDistance;
//    const float AcceptableRadius = ControlledAlly->AcceptableRadius;
//
//    // --- 近すぎたら後退 ---
//    if (Distance < TooCloseDistance)
//    {
//        FVector AwayDir = (ControlledAlly->GetActorLocation() - TargetEnemy->GetActorLocation()).GetSafeNormal();
//        FVector RetreatPos = ControlledAlly->GetActorLocation() + AwayDir * 300.f;
//        MoveToLocation(RetreatPos, AcceptableRadius);
//        return;
//    }
//
//    // --- 離れすぎていれば前進 ---
//    if (FMath::Abs(Distance - MaintainDistance) > AcceptableRadius)
//    {
//        FVector ToTarget = (TargetEnemy->GetActorLocation() - ControlledAlly->GetActorLocation()).GetSafeNormal();
//        FVector DesiredPos = TargetEnemy->GetActorLocation() - ToTarget * MaintainDistance;
//        MoveToLocation(DesiredPos, AcceptableRadius);
//    }
//    else
//    {
//        StopMovement();
//    }
//}
//
//void AAllyAIController::FaceEnemy(float DeltaSeconds)
//{
//    if (!TargetEnemy || !ControlledAlly) return;
//
//    FVector ToEnemy = TargetEnemy->GetActorLocation() - ControlledAlly->GetActorLocation();
//    ToEnemy.Z = 0.f;
//    FRotator LookRot = FRotationMatrix::MakeFromX(ToEnemy).Rotator();
//    ControlledAlly->SetActorRotation(
//        FMath::RInterpTo(ControlledAlly->GetActorRotation(), LookRot, DeltaSeconds, 5.f)
//    );
//}
