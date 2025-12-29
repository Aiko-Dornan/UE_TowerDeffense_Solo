// EnemyAIController.cpp

//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//#include "EnemyCharacterBase.h"
//#include "DefenseBase.h"
//
//AEnemyAIController::AEnemyAIController()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController BeginPlay"));
//}
//
//void AEnemyAIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//
//    if (AEnemyCharacterBase* Enemy = Cast<AEnemyCharacterBase>(InPawn))
//    {
//        CurrentEnemy = Enemy;
//
//        // 初期ターゲットを設定
//        CurrentTarget = CurrentEnemy->GetCurrentTarget();
//        if (!IsValid(CurrentTarget))
//        {
//            CurrentTarget = CurrentEnemy->ChooseTarget_Default();
//            if (IsValid(CurrentTarget))
//            {
//                CurrentEnemy->SetCurrentTarget(CurrentTarget);
//            }
//        }
//
//        if (IsValid(CurrentTarget))
//        {
//            MoveToActor(CurrentTarget, CurrentEnemy->GetEffectiveAttackRange(CurrentTarget));
//            SetFocus(CurrentTarget);
//            UE_LOG(LogTemp, Warning, TEXT("EnemyAIController initial target: %s"), *CurrentTarget->GetName());
//        }
//    }
//}
//
//void AEnemyAIController::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    UpdateTargetAndMove();
//}
//
//void AEnemyAIController::UpdateTargetAndMove()
//{
//    if (!IsValid(CurrentEnemy))
//        return;
//
//    // 現在のターゲットが破壊されている場合、再選択
//    if (!IsValid(CurrentTarget))
//    {
//        CurrentTarget = CurrentEnemy->ChooseTarget_Default();
//        CurrentEnemy->SetCurrentTarget(CurrentTarget);
//
//        if (IsValid(CurrentTarget))
//        {
//            MoveToActor(CurrentTarget, CurrentEnemy->GetEffectiveAttackRange(CurrentTarget)-200.0f);
//            SetFocus(CurrentTarget);
//            UE_LOG(LogTemp, Warning, TEXT("EnemyAIController new target: %s"), *CurrentTarget->GetName());
//        }
//        return;
//    }
//
//    // ターゲットは有効、移動先やフォーカスを更新（必要に応じて）
//    float Distance = CurrentEnemy->GetDistanceTo(CurrentTarget);
//    float AttackRange = CurrentEnemy->GetEffectiveAttackRange(CurrentTarget);
//
//    if (Distance > AttackRange)
//    {
//        MoveToActor(CurrentTarget, AttackRange-200.0f);
//    }
//
//    
//    if (!CurrentTarget)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("%s: ChooseTarget_Default returned nullptr"), *GetName());
//    }
//}




#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include "DefenseBase.h"
#include "MyHeroPlayer.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController BeginPlay"));

    // 防衛対象を探す
    TArray<AActor*> FoundBases;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);

    if (FoundBases.Num() > 0)
    {
        TargetActor = FoundBases[0]; // 最初の防衛対象を目的地に
        MoveToActor(TargetActor, 0.0f);
        UE_LOG(LogTemp, Warning, TEXT("EnemyAI target set to DefenseBase: %s"), *TargetActor->GetName());
    }

}

//void AEnemyAIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//
//    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController possessed: %s"), *InPawn->GetName());
//
//    if (PlayerPawn && InPawn)
//    {
//        MoveToActor(PlayerPawn, 0.0f); // 到達半径100
//    }
//}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController possessed: %s"), *InPawn->GetName());

    if (AEnemyCharacterBase* Enemy = Cast<AEnemyCharacterBase>(InPawn))
    {
        Enemy->UpdateTarget();        // ターゲット選定
        Enemy->StartMovingToTarget(); // 移動開始
    }

    //if (AEnemyCharacterBase* Enemy = Cast<AEnemyCharacterBase>(InPawn))
    //{
    //    if (AActor* Target = Enemy->GetCurrentTarget()) // ← Getter を追加
    //    {
    //        MoveToActor(Target, Enemy->GetEffectiveAttackRange(Target)-200.0f);
    //        SetFocus(Target);
    //    }
    //    else
    //    {
    //        // 念のためここでもターゲット検索
    //        AActor* NewTarget = Enemy->ChooseTargetBP();
    //        if (IsValid(NewTarget))
    //        {
    //            Enemy->SetCurrentTarget(NewTarget);
    //            MoveToActor(NewTarget, Enemy->GetEffectiveAttackRange(NewTarget)-200.0f);
    //            SetFocus(NewTarget);
    //        }
    //    }
    //}
}



//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    AEnemyCharacterBase* Enemy = Cast<AEnemyCharacterBase>(GetPawn());
//    if (!Enemy) return;
//
//    AActor* Target = Enemy->ChooseTargetBP();
//    if (Target)
//    {
//        MoveToActor(Target, 50.f);
//    }
//
//}


//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//
//AEnemyAIController::AEnemyAIController()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController BeginPlay"));
//}
//
//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    //UE_LOG(LogTemp, Warning, TEXT("EnemyAIController Tick"));
//
//    if (PlayerPawn)
//    {
//        MoveToActor(PlayerPawn, 100.0f);
//    }
//}
//
//void AEnemyAIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//
//    UE_LOG(LogTemp, Warning, TEXT("AIController possessed pawn: %s"), *InPawn->GetName());
//
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//
//    if (PlayerPawn)
//    {
//        MoveToActor(PlayerPawn, 100.0f);
//    }
//}


//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//#include "NavigationSystem.h"
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//}
//
//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    if (!PlayerPawn) return;
//
//    MoveToActor(PlayerPawn, 100.0f); // 100cm以内で停止
//
//    //  MoveToActor の結果を変数で受け取る
//    EPathFollowingRequestResult::Type MoveResult = MoveToActor(PlayerPawn, 100.0f);
//
//    switch (MoveResult)
//    {
//    case EPathFollowingRequestResult::Failed:
//        UE_LOG(LogTemp, Warning, TEXT("AI MoveToActor FAILED: %s"), *GetName());
//        break;
//
//    case EPathFollowingRequestResult::RequestSuccessful:
//        UE_LOG(LogTemp, Warning, TEXT("AI moving towards player..."));
//        break;
//
//    default:
//        UE_LOG(LogTemp, Warning, TEXT("AI unknown MoveResult."));
//        break;
//    }
//}


//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//#include "GameFramework/Character.h"
//#include "NavigationSystem.h"
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//}
//
//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    if (PlayerPawn)
//    {
//        MoveToActor(PlayerPawn, 3.f); // 5.fは到達半径
//    }
//}
