#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "MyHeroPlayer.h"
#include "AllyCharacter.h"
#include "DefenseBase.h"
#include "TD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtr.h"
#include "DefenseStructure.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemySpawnerWave.h"

// ==================== コンストラクタ ====================

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackRange = 150.0f;
    AttackCooldown = 2.0f;
    AttackDamage = 10.0f;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = 300.f;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// ==================== BeginPlay ====================

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    OnActorBeginOverlap.AddDynamic(this, &AEnemyCharacterBase::OnEnemyBeginOverlap);

    while (!GetController())
    {
        SpawnDefaultController();
    }

    PlayerCharacter = Cast<AMyHeroPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

    while(!IsValid(BaseStructure))
    {
        TArray<AActor*> FoundBases;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
        if (FoundBases.Num() > 0)
        {
            BaseStructure = Cast<ADefenseBase>(FoundBases[0]);
        }
    }

    CurrentHealth = MaxHealth;

    // 最初のターゲットは必ず基地
    if (IsValid(BaseStructure))
    {
        //UE_LOG(LogTemp, Warning, TEXT("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ"));

        CurrentTarget = BaseStructure;

        
    }
    else
    {
       // UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
        // 基地が存在しない場合は距離判定ターゲットを探す
        CurrentTarget = ChooseTarget_Default();
    }

    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    {
        AI->SetFocus(BaseStructure);
        AI->MoveToActor(BaseStructure, 5.f);
    }

    ////  最初のターゲットを設定（これが抜けていた）
    //CurrentTarget = ChooseTarget_Default();
    ////CurrentTarget = BaseStructure;

    //if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    //{
    //    if (IsValid(CurrentTarget))
    //    {
    //        AI->SetFocus(CurrentTarget);
    //        AI->MoveToActor(CurrentTarget, 5.f);
    //    }
    //}

    // //定期的に停止判定を実施
    //GetWorldTimerManager().SetTimerForNextTick([this]()
    //    {
    //        GetWorldTimerManager().SetTimer(
    //            StopCheckTimerHandle,
    //            this,
    //            &AEnemyCharacterBase::CheckIfStopped,
    //            StopCheckInterval,
    //            true);
    //    });

    // ターゲット更新タイマーを開始
    GetWorldTimerManager().SetTimer(TargetUpdateTimerHandle, this, &AEnemyCharacterBase::UpdateTarget, TargetUpdateInterval, true);

}




//void AEnemyCharacterBase::BeginPlay()
//{
//    Super::BeginPlay();
//
//    OnActorBeginOverlap.AddDynamic(this, &AEnemyCharacterBase::OnEnemyBeginOverlap);
//
//    while (!GetController())
//    {
//        SpawnDefaultController();
//    }
//
//    PlayerCharacter = Cast<AMyHeroPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
//
//    TArray<AActor*> FoundBases;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
//    if (FoundBases.Num() > 0)
//    {
//        BaseStructure = Cast<ADefenseBase>(FoundBases[0]);
//    }
//
//    CurrentHealth = MaxHealth;
//}

// ==================== Tick ====================

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead) return;
    if (!IsValid(CurrentTarget)) return;

    // ==========================
    // 現在ターゲットへの直線上にバリケードがあるかチェック
    // ==========================
    AActor* BlockingStructure = CheckBlockingStructure(CurrentTarget);
    bool bUseDirectMove = false;

    if (BlockingStructure)
    {
        if (BlockingStructure != CurrentTarget)
        {
            PreviousTarget = CurrentTarget;
            CurrentTarget = BlockingStructure;

            // 建物破壊通知を受け取る
            if (ADefenseStructure* Struct = Cast<ADefenseStructure>(BlockingStructure))
            {
                Struct->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);
            }
        }

        bUseDirectMove = true; // バリケード直進フラグ
    }

    // ==========================
    // 攻撃距離チェック
    // ==========================
    FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
    float Distance = ToTarget.Size();
    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
    float DesiredDistance = EffectiveAttackRange + 10.0f; // ターゲットより10f外側を維持
    FVector Direction = ToTarget.GetSafeNormal();

    // ==========================
    // 移動処理
    // ==========================
    if (Distance > DesiredDistance)
    {
        float MoveDistance = FMath::Min(GetCharacterMovement()->MaxWalkSpeed * DeltaTime, Distance - DesiredDistance);
        FVector NewLocation = GetActorLocation() + Direction * MoveDistance;
        SetActorLocation(NewLocation, true);
    }

    SetActorRotation(Direction.Rotation());

    // ==========================
    // 攻撃
    // ==========================
    if (Distance <= EffectiveAttackRange)
    {
        PerformAttack();
        return;
    }

    // ==========================
    // AI 移動はバリケード以外のみ
    // ==========================
    if (!bUseDirectMove)
    {
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            AI->SetFocus(CurrentTarget);
            AI->MoveToActor(CurrentTarget, GetCharacterMovement()->MaxWalkSpeed);
        }
    }
}




//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//    if (bIsDead) return;
//
//    // ==========================
//    // 現在ターゲットが無効なら、まずターゲット選定
//    // ==========================
//    if (!IsValid(CurrentTarget))
//    {
//        UpdateTarget(); // プレイヤー・味方・基地を優先で選ぶ関数
//    }
//
//    if (!IsValid(CurrentTarget)) return;
//
//    // ==========================
//    // 現在ターゲットへの直線上にバリケードがあるかチェック
//    // ==========================
//    AActor* BlockingStructure = CheckBlockingStructure(CurrentTarget);
//    if (BlockingStructure && BlockingStructure != CurrentTarget)
//    {
//        // まだバリケードを狙っていなければ切り替え
//        PreviousTarget = CurrentTarget;
//        CurrentTarget = BlockingStructure;
//
//        if (ADefenseStructure* Struct = Cast<ADefenseStructure>(BlockingStructure))
//        {
//            Struct->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);
//        }
//
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->SetFocus(CurrentTarget);
//           // AI->MoveToActor(CurrentTarget, 5.f);
//        }
//    }
//
//    // ==========================
//    // 攻撃距離チェック
//    // ==========================
//    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
//    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
//
//    if (Distance <= EffectiveAttackRange)
//    {
//        PerformAttack();
//        return;
//    }
//
//    // ==========================
//    // 移動
//    // ==========================
//        if (/*CurrentTarget == BaseStructure*/law_inteli_flag==true)
//    {
//       // UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
//        FVector Direction = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//        FVector NewLocation = GetActorLocation() + Direction * GetCharacterMovement()->MaxWalkSpeed * DeltaTime;
//        SetActorLocation(NewLocation, true);
//       // SetActorRotation(Direction.Rotation());
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->SetFocus(CurrentTarget);
//           
//        }
//    }
//    else
//    {
//        //UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
//
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->SetFocus(CurrentTarget);
//            AI->MoveToActor(CurrentTarget, GetCharacterMovement()->MaxWalkSpeed);
//        }
//    }
//}


//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//    if (bIsDead) return;
//    if (!IsValid(CurrentTarget)) return;
//
//    // ==========================
//    // 現在ターゲットへの直線上にバリケードがあるかチェック
//    // ==========================
//    AActor* BlockingStructure = CheckBlockingStructure(CurrentTarget);
//    if (BlockingStructure)
//    {
//        // すでにバリケードを狙っていない場合のみターゲット切り替え
//        if (BlockingStructure != CurrentTarget)
//        {
//            PreviousTarget = CurrentTarget;
//            //CurrentTarget = BlockingStructure;
//
//            // 建物破壊通知を受け取って復帰
//            if (ADefenseStructure* Struct = Cast<ADefenseStructure>(BlockingStructure))
//            {
//                Struct->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);
//            }
//
//            if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//            {
//                AI->SetFocus(CurrentTarget);
//                AI->MoveToActor(CurrentTarget, 5.f);
//            }
//        }
//    }
//
//    // ==========================
//    // 攻撃距離チェック
//    // ==========================
//    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
//    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
//
//    if (Distance <= EffectiveAttackRange)
//    {
//        PerformAttack();
//        return;
//    }
//
//    // ==========================
//    // 基地の場合は直進、それ以外はAI移動
//    // ==========================
//    if (/*CurrentTarget == BaseStructure*/law_inteli_flag==true)
//    {
//       // UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
//        FVector Direction = (BaseStructure->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//        FVector NewLocation = GetActorLocation() + Direction * GetCharacterMovement()->MaxWalkSpeed * DeltaTime;
//        SetActorLocation(NewLocation, true);
//       // SetActorRotation(Direction.Rotation());
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->SetFocus(CurrentTarget);
//           
//        }
//    }
//    else
//    {
//        //UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
//
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->SetFocus(CurrentTarget);
//            AI->MoveToActor(CurrentTarget, GetCharacterMovement()->MaxWalkSpeed);
//        }
//    }
//}
//

//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//    if (bIsDead) return;
//
//    //CurrentTarget = ChooseTarget_Default();
//
//    //if (!IsValid(CurrentTarget))
//    //{
//    //    CurrentTarget = ChooseTarget_Default();
//
//    //    // それでも見つからない場合、基地をターゲットにする
//    //    if (!IsValid(CurrentTarget) && IsValid(BaseStructure))
//    //    {
//    //        UE_LOG(LogTemp, Warning, TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
//    //        CurrentTarget = BaseStructure;
//
//    //        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//    //        {
//    //            
//    //                AI->SetFocus(BaseStructure);
//    //                AI->MoveToActor(BaseStructure, 5.f);
//    //            
//    //        }
//    //        
//    //    }
//
//    //    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//    //    {
//    //        if (IsValid(CurrentTarget))
//    //        {
//    //            AI->SetFocus(CurrentTarget);
//    //            AI->MoveToActor(CurrentTarget, 5.f);
//    //        }
//    //    }
//    //}
//   
//       // ターゲットが存在しない or 無効なら基地へ戻す
//    if (!IsValid(CurrentTarget))
//    {
//        if (IsValid(BaseStructure))
//        {
//            CurrentTarget = BaseStructure;
//        }
//        else
//        {
//            CurrentTarget = ChooseTarget_Default();
//        }
//
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            if (IsValid(CurrentTarget))
//            {
//                AI->SetFocus(CurrentTarget);
//                AI->MoveToActor(CurrentTarget, 5.f);
//            }
//        }
//    }
//
//    //// --- ターゲットが存在しないか壊れていたら選び直し ---
//    //if (!IsValid(CurrentTarget))
//    //{
//    //    CurrentTarget = ChooseTarget_Default();
//    //    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//    //    {
//    //        if (IsValid(CurrentTarget))
//    //        {
//    //            AI->SetFocus(CurrentTarget);
//    //            AI->MoveToActor(CurrentTarget, 5.f);
//    //        }
//    //    }
//    //}
//
//    /*if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//    {
//        AI->SetFocus(CurrentTarget);
//        AI->MoveToActor(CurrentTarget, 5.f);
//    }
//
//    CurrentTarget = ChooseTarget_Default();*/
//
//    if (!IsValid(CurrentTarget)) return;
//
//    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
//    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
//
//    if (Distance <= EffectiveAttackRange)
//    {
//        PerformAttack();
//    }
//    else
//    {
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->SetFocus(CurrentTarget);
//            AI->MoveToActor(CurrentTarget, 5.f);
//        }
//    }
//}

//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (bIsDead || !IsValid(CurrentTarget)) return;
//
//    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
//    const float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
//
//    if (Distance <= EffectiveAttackRange)
//    {
//        PerformAttack();
//    }
//}

AActor* AEnemyCharacterBase::CheckBlockingStructure(AActor* MainTarget)
{
    if (!IsValid(MainTarget)) return nullptr;

    UCapsuleComponent* Capsule = GetCapsuleComponent();
    float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;

    FVector Start = GetActorLocation() - FVector(0, 0, HalfHeight - 10.f);
    FVector End = MainTarget->GetActorLocation() + FVector(0, 0, 50.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 0.1f, 0, 1.5f);

    if (bHit)
    {
        if (ADefenseStructure* HitStructure = Cast<ADefenseStructure>(Hit.GetActor()))
        {
            // 目標より手前のバリケードのみ有効
            float DistToHit = FVector::Dist(GetActorLocation(), HitStructure->GetActorLocation());
            float DistToTarget = FVector::Dist(GetActorLocation(), MainTarget->GetActorLocation());

            if (DistToHit < DistToTarget)
                return HitStructure;
        }
    }

    return nullptr;
}


void AEnemyCharacterBase::UpdateTarget()
{
    if (bIsDead) return;

    // ---  ターゲットの基本選定 ---
    AActor* NewTarget = ChooseTarget_Default(); // バリケード以外を含む通常のロジック

    // 無効ターゲットなら基地へフォールバック
    if (!IsValid(NewTarget))
    {
        if (IsValid(BaseStructure))
            NewTarget = BaseStructure;
        else
            return;
    }

    // ---  新ターゲットがバリケードでない場合のみ遮蔽物をチェック ---
    AActor* BlockingStructure = nullptr;
    if (!NewTarget->IsA(ADefenseStructure::StaticClass()))
    {
        BlockingStructure = CheckBlockingStructure(NewTarget);

        if (IsValid(BlockingStructure))
        {
            PreviousTarget = NewTarget;
            NewTarget = BlockingStructure;

            // 破壊通知
            BlockingStructure->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);
        }
    }

    // ---  ターゲット変更が必要な場合のみ更新 ---
    if (NewTarget != CurrentTarget)
    {
        CurrentTarget = NewTarget;

        //law_inteli_flag = false;

        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            AI->ClearFocus(EAIFocusPriority::Gameplay);
            AI->StopMovement();

            if (IsValid(CurrentTarget)&&!law_inteli_flag)
            {
                AI->SetFocus(CurrentTarget);
                AI->MoveToActor(CurrentTarget, 5.f);
            }
        }
    }
}



//void AEnemyCharacterBase::UpdateTarget()
//{
//    if (bIsDead) return;
//
//    // ターゲット無効なら再設定
//    if (!IsValid(CurrentTarget))
//    {
//        CurrentTarget = ChooseTarget_Default();
//        return;
//    }
//
//    if (GetWorld()->GetTimeSeconds() - LastBarrierCheckTime < BarrierCheckCooldown) return;
//    LastBarrierCheckTime = GetWorld()->GetTimeSeconds();
//
//    // --- 現ターゲットがバリケードではない場合のみチェック ---
//    if (!CurrentTarget->IsA(ADefenseStructure::StaticClass()))
//    {
//        // バリケードが視線上にあるかチェック
//        if (AActor* BlockStructure = CheckBlockingStructure(CurrentTarget))
//        {
//            float DistToBlock = FVector::Dist(GetActorLocation(), BlockStructure->GetActorLocation());
//            float DistToTarget = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
//
//            // ターゲットより手前にある && 近すぎない場合のみターゲット変更
//            if (DistToBlock < DistToTarget/* && DistToBlock < 2000.f*/)
//            {
//                PreviousTarget = CurrentTarget;
//                CurrentTarget = BlockStructure;
//
//                // バリケードが破壊されたら戻る
//                BlockStructure->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);
//            }
//        }
//    }
//
//    // --- AIの更新 ---
//   /* if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//    {
//        if (IsValid(CurrentTarget))
//        {
//            AI->SetFocus(CurrentTarget);
//            AI->MoveToActor(CurrentTarget, 5.f);
//        }
//    }*/
//}



//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (bIsDead) return;
//
//    // ターゲットがいなければ選び直す
//    if (!IsValid(CurrentTarget))
//    {
//        CurrentTarget = ChooseTarget_Default();
//    }
//
//    if (!IsValid(CurrentTarget)) return;
//
//    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
//    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
//
//    if (Distance <= EffectiveAttackRange)
//    {
//        PerformAttack();
//    }
//    else
//    {
//        // 攻撃範囲外 → ターゲットへ移動
//        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
//        {
//            AI->MoveToActor(CurrentTarget, 5.f);
//        }
//    }
//}

// ==================== オーバーラップイベント ====================

void AEnemyCharacterBase::OnEnemyBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (bIsDead || !IsValid(OtherActor)) return;

    // --- 防衛建物にぶつかったらターゲット変更 ---
    if (ADefenseStructure* HitStructure = Cast<ADefenseStructure>(OtherActor))
    {
        if (CurrentTarget == HitStructure) return;

        UE_LOG(LogTemp, Warning, TEXT("%s collided with structure %s -> targeting it!"),
            *GetName(), *HitStructure->GetName());

        PreviousTarget = CurrentTarget;
        CurrentTarget = HitStructure;

        // 建物が破壊されたら通知を受け取る
        HitStructure->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);

        /*if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            if (IsValid(CurrentTarget) && CurrentTarget != BaseStructure)
            {
                AI->SetFocus(CurrentTarget);
                AI->MoveToActor(CurrentTarget, 5.f);
            }
        }*/
        return;
    }

    // --- プレイヤーや味方、基地など ---
    if (AMyHeroPlayer* Player = Cast<AMyHeroPlayer>(OtherActor))
    {
        CurrentTarget = Player;
        return;
    }

    if (AAllyCharacter* Ally = Cast<AAllyCharacter>(OtherActor))
    {
        CurrentTarget = Ally;
        return;
    }

    if (ADefenseBase* Base = Cast<ADefenseBase>(OtherActor))
    {
        CurrentTarget = Base;
        return;
    }
}

// ==================== ターゲット破壊通知 ====================

void AEnemyCharacterBase::OnTargetDestroyed(AActor* DestroyedActor)
{
    if (bIsDead) return;

    if (CurrentTarget == DestroyedActor)
    {
        CurrentTarget = nullptr;

        // 元ターゲット（プレイヤー・基地など）に戻る
        if (IsValid(PreviousTarget))
        {
            CurrentTarget = PreviousTarget;
            PreviousTarget = nullptr;
        }
        else
        {
            CurrentTarget = ChooseTarget_Default();
        }

        // --- フォーカスをリセット ---
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            AI->ClearFocus(EAIFocusPriority::Gameplay);
            AI->StopMovement();

            if (IsValid(CurrentTarget)&&!law_inteli_flag)
            {
                AI->SetFocus(CurrentTarget);
                AI->MoveToActor(CurrentTarget, 5.f);
            }
        }
    }
}


// ==================== ターゲット選定（Blueprint拡張対応） ====================

AActor* AEnemyCharacterBase::ChooseTargetBP_Implementation()
{
    return ChooseTarget_Default();
}

AActor* AEnemyCharacterBase::ChooseTarget_Default()
{
    // 停止中なら認識範囲を2倍に
    const float MaxConsiderRangeBase = bIsRecognitionExtended ? ExtendedConsiderRange : DefaultConsiderRange;
    const float MaxConsiderRange = 8000.0f;

    const float BlockCheckAngle = 45.f;

    TArray<FTargetCandidate> Candidates;

    // --- プレイヤー ---
    if (IsValid(PlayerCharacter))
    {
        const float Dist = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
        if (Dist < MaxConsiderRange)
        {
            FTargetCandidate Candidate;
            Candidate.Actor = PlayerCharacter;
            Candidate.Score = PlayerPriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
            Candidates.Add(Candidate);
        }
    }

    // --- 味方キャラ ---
    TArray<AActor*> FoundAllies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);
    for (AActor* Ally : FoundAllies)
    {
        if (!IsValid(Ally)) continue;
        const float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
        if (Dist < MaxConsiderRange)
        {
            FTargetCandidate Candidate;
            Candidate.Actor = Ally;
            Candidate.Score = AllyPriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
            Candidates.Add(Candidate);
        }
    }

    // --- 基地（常に取得）---
    ADefenseBase* DefenseBase = nullptr;
    {
        TArray<AActor*> FoundBases;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
        if (FoundBases.Num() > 0)
        {
            DefenseBase = Cast<ADefenseBase>(FoundBases[0]);
            if (IsValid(DefenseBase))
            {
                const float Dist = FVector::Dist(GetActorLocation(), DefenseBase->GetActorLocation());
                FTargetCandidate Candidate;
                Candidate.Actor = DefenseBase;
                Candidate.Score = BasePriority * FMath::Clamp(1.f - (Dist / (MaxConsiderRangeBase * 1.2f)), 0.f, 1.f);
                Candidates.Add(Candidate);
            }
        }
    }

    // --- 防衛建物（進行ルート上のみ）---
    if (IsValid(DefenseBase))
    {
        TArray<AActor*> FoundStructures;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseStructure::StaticClass(), FoundStructures);

        FVector ToBaseDir = (DefenseBase->GetActorLocation() - GetActorLocation()).GetSafeNormal();

        for (AActor* StructureActor : FoundStructures)
        {
            if (!IsValid(StructureActor)) continue;

            FVector ToStructureDir = (StructureActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ToBaseDir, ToStructureDir)));

            if (Angle < BlockCheckAngle)
            {
                FHitResult Hit;
                FCollisionQueryParams Params;
                Params.AddIgnoredActor(this);

                bool bHit = GetWorld()->LineTraceSingleByChannel(
                    Hit,
                    GetActorLocation() + FVector(0, 0, 50.f),
                    StructureActor->GetActorLocation() + FVector(0, 0, 50.f),
                    ECC_Visibility, Params);

                if (bHit && Hit.GetActor() == StructureActor)
                {
                    const float Dist = FVector::Dist(GetActorLocation(), StructureActor->GetActorLocation());
                    FTargetCandidate Candidate;
                    Candidate.Actor = StructureActor;
                    Candidate.Score = StructurePriority * FMath::Clamp(1.f - (Dist / (MaxConsiderRange * 0.5f)), 0.f, 1.f);
                    Candidates.Add(Candidate);

                    //UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
                    
                }
                
                //law_inteli_flag = true;

            }
           /* else
            {
                law_inteli_flag = false;
            }*/
        }
    }

    // --- ソート ---
    Candidates.Sort([](const FTargetCandidate& A, const FTargetCandidate& B)
        {
            return A.Score > B.Score;
        });

    // --- フォールバック処理 ---
    if (Candidates.Num() > 0)
    {
        return Candidates[0].Actor;
    }

    //// ターゲットが一切見つからない場合は必ず基地をターゲットにする
    //if (!IsValid(BaseStructure))
    //{
    //    TArray<AActor*> FoundBases;
    //    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
    //    if (FoundBases.Num() > 0)
    //    {
    //        BaseStructure = Cast<ADefenseBase>(FoundBases[0]);
    //    }
    //}

    //return IsValid(BaseStructure) ? BaseStructure : nullptr;

    return Candidates[0].Actor;
}

void AEnemyCharacterBase::CheckIfStopped()
{
    if (bIsDead) return;

    const float Speed = GetVelocity().Size();
    if (CurrentTarget==BaseStructure) {

        if (Speed < StopThresholdSpeed)
        {
            TimeSinceLastMove += StopCheckInterval;

            // 一定時間停止していたら認識距離を拡張
            if (TimeSinceLastMove >= StopDurationToExtend && !bIsRecognitionExtended)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s stopped  extending recognition range!"), *GetName());
                bIsRecognitionExtended = true;
            }
        }
        else
        {
            // 動いてるならリセット
            TimeSinceLastMove = 0.f;
            bIsRecognitionExtended = false;
        }

    }
    
}

//AActor* AEnemyCharacterBase::ChooseTarget_Default()
//{
//    const float MaxConsiderRange = 6000.f; // これ以上離れたものは無視
//    const float BlockCheckAngle = 45.f;    // ±45度の範囲を「進行ルート上」とみなす
//
//    TArray<FTargetCandidate> Candidates;
//
//    // --- プレイヤー ---
//    if (IsValid(PlayerCharacter))
//    {
//        const float Dist = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
//        if (Dist < MaxConsiderRange)
//        {
//            FTargetCandidate Candidate;
//            Candidate.Actor = PlayerCharacter;
//            Candidate.Score = PlayerPriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
//            Candidates.Add(Candidate);
//        }
//    }
//
//    // --- 味方キャラ ---
//    TArray<AActor*> FoundAllies;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);
//    for (AActor* Ally : FoundAllies)
//    {
//        if (!IsValid(Ally)) continue;
//        const float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
//        if (Dist < MaxConsiderRange)
//        {
//            FTargetCandidate Candidate;
//            Candidate.Actor = Ally;
//            Candidate.Score = AllyPriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
//            Candidates.Add(Candidate);
//        }
//    }
//
//    // --- 基地 ---
//    ADefenseBase* DefenseBase = nullptr;
//    {
//        TArray<AActor*> FoundBases;
//        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
//        if (FoundBases.Num() > 0)
//        {
//            DefenseBase = Cast<ADefenseBase>(FoundBases[0]);
//            if (IsValid(DefenseBase))
//            {
//                const float Dist = FVector::Dist(GetActorLocation(), DefenseBase->GetActorLocation());
//                FTargetCandidate Candidate;
//                Candidate.Actor = DefenseBase;
//                Candidate.Score = BasePriority * FMath::Clamp(1.f - (Dist / (MaxConsiderRange * 1.2f)), 0.f, 1.f);
//                Candidates.Add(Candidate);
//            }
//        }
//    }
//
//    // --- 防衛建物（進行ルート上のみ） ---
//    if (IsValid(DefenseBase))
//    {
//        TArray<AActor*> FoundStructures;
//        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseStructure::StaticClass(), FoundStructures);
//
//        FVector ToBaseDir = (DefenseBase->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//
//        for (AActor* StructureActor : FoundStructures)
//        {
//            if (!IsValid(StructureActor)) continue;
//
//            FVector ToStructureDir = (StructureActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//            float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ToBaseDir, ToStructureDir)));
//
//            if (Angle < BlockCheckAngle)
//            {
//                FHitResult Hit;
//                FCollisionQueryParams Params;
//                Params.AddIgnoredActor(this);
//
//                bool bHit = GetWorld()->LineTraceSingleByChannel(
//                    Hit,
//                    GetActorLocation() + FVector(0, 0, 50.f),
//                    StructureActor->GetActorLocation() + FVector(0, 0, 50.f),
//                    ECC_Visibility, Params);
//
//                if (bHit && Hit.GetActor() == StructureActor)
//                {
//                    const float Dist = FVector::Dist(GetActorLocation(), StructureActor->GetActorLocation());
//                    FTargetCandidate Candidate;
//                    Candidate.Actor = StructureActor;
//                    Candidate.Score = StructurePriority * FMath::Clamp(1.f - (Dist / (MaxConsiderRange * 0.5f)), 0.f, 1.f);
//                    Candidates.Add(Candidate);
//                }
//            }
//        }
//    }
//
//    // --- スコア順でソート ---
//    Candidates.Sort([](const FTargetCandidate& A, const FTargetCandidate& B)
//        {
//            return A.Score > B.Score;
//        });
//
//    // --- デバッグ出力 ---
//    if (Candidates.Num() > 0)
//    {
//        //UE_LOG(LogTemp, Log, TEXT("Target: %s (Score=%.2f)"),
//            //*Candidates[0].Actor->GetName(), Candidates[0].Score);
//    }
//
//    return Candidates.Num() > 0 ? Candidates[0].Actor : BaseStructure;
//}


// ==================== 攻撃処理 ====================

void AEnemyCharacterBase::PerformAttack()
{
    if (!bCanAttack || bIsDead || !CurrentTarget) return;

    bCanAttack = false;

    UE_LOG(LogTemp, Warning, TEXT("%s attacks %s!"),
        *GetName(), *CurrentTarget->GetName());

    UGameplayStatics::ApplyDamage(CurrentTarget, AttackDamage, GetController(), this, nullptr);

    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this,
        &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
}

// ==================== 被ダメージ処理 ====================

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

    UpdateTarget();

    return ActualDamage;
}

// ==================== 死亡処理 ====================

void AEnemyCharacterBase::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died!"), *GetName());

    if (!bNotifiedSpawner)
    {
        bNotifiedSpawner = true;
        if (AEnemySpawnerWave* Spawner = Cast<AEnemySpawnerWave>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawnerWave::StaticClass())))
        {
            Spawner->NotifyEnemyDestroyed(this);
        }
    }

    Destroy();
}

// ==================== 攻撃レンジ設定 ====================

float AEnemyCharacterBase::GetEffectiveAttackRange(AActor* Target) const
{
    if (Target->IsA(ADefenseBase::StaticClass()))
    {
        return 400.f; // 基地には広いレンジ
    }
    else if (Target->IsA(ADefenseStructure::StaticClass()))
    {
        return 200.f; // 建物はやや広め
    }
    else if (Target->IsA(AAllyCharacter::StaticClass()) || Target->IsA(AMyHeroPlayer::StaticClass()))
    {
        return 150.f;
    }

    return AttackRange;
}


//#include "EnemyCharacterBase.h"
//#include "EnemyAIController.h"
//#include "MyHeroPlayer.h"
//#include"AllyCharacter.h"
//#include"DefenseBase.h"
//#include"TD_GameModeBase.h"
//#include "Kismet/GameplayStatics.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "TimerManager.h"
//#include "UObject/WeakObjectPtr.h"
//
//#include "DefenseStructure.h"
//#include "NavigationSystem.h"
//#include "DrawDebugHelpers.h"
//#include "Kismet/KismetMathLibrary.h"
//
//AEnemyCharacterBase::AEnemyCharacterBase()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AttackRange = 150.0f;
//    AttackCooldown = 2.0f;
//    AttackDamage = 10.0f;
//    MaxHealth = 100.0f;
//    CurrentHealth = MaxHealth;
//
//    GetCharacterMovement()->MaxWalkSpeed = 200.f;
//
//    AIControllerClass = AEnemyAIController::StaticClass();
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//
//
//}
//
//void AEnemyCharacterBase::BeginPlay()
//{
//    Super::BeginPlay();
//
//    while(!GetController())
//    {
//        SpawnDefaultController();
//    }
//
//    PlayerCharacter = Cast<AMyHeroPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
//
//    TArray<AActor*> FoundBases;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
//    if (FoundBases.Num() > 0)
//    {
//        BaseStructure = Cast<ADefenseBase>(FoundBases[0]);
//    }
//
//    CurrentHealth = MaxHealth;
//}
//
//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (bIsDead || !PlayerCharacter) return;
//
//    // 1. 攻撃対象を選ぶ
//    AActor* Target = ChooseTarget();
//
//    if (!Target) return;
//
//    CurrentTarget = Target;
//
//
//    // 2. 距離チェック
//    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
//
//    // ターゲットのタイプによって攻撃レンジを変える
//
//    float EffectiveAttackRange = GetEffectiveAttackRange(Target);
//
//    if (Distance <= EffectiveAttackRange)
//    {
//        PerformAttack();
//    }
//
//    ////float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
//    //if (Distance <= AttackRange)
//    //{
//    //    PerformAttack();
//    //}
//
//    
//
//}
//
//// --- BlueprintNativeEvent のデフォルト実装 ---
//AActor* AEnemyCharacterBase::ChooseTargetBP_Implementation()
//{
//    return ChooseTarget(); // デフォルトはC++のChooseTarget()に委譲
//}
//
//AActor* AEnemyCharacterBase::ChooseTarget()
//{
//    // --- プレイヤーと味方キャラを取得 ---
//    TArray<AActor*> PotentialTargets;
//    if (PlayerCharacter)
//    {
//        PotentialTargets.Add(PlayerCharacter);
//    }
//
//    TArray<AActor*> FoundAllies;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);
//    PotentialTargets.Append(FoundAllies);
//
//    // --- 基地を取得 ---
//    TArray<AActor*> FoundBases;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
//    AActor* DefenseBase = (FoundBases.Num() > 0) ? FoundBases[0] : nullptr;
//
//    // --- 候補とスコアを格納 ---
//    TArray<FTargetCandidate> Candidates;
//
//    // プレイヤー/味方優先
//    const float PlayerPriorityRange = 2000.f;
//
//    for (AActor* Target : PotentialTargets)
//    {
//        if (!IsValid(Target)) continue;
//
//        float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
//        if (Dist <= PlayerPriorityRange)
//        {
//            // 候補に追加
//            FTargetCandidate Candidate;
//            Candidate.Actor = Target;
//            Candidate.Score = 100.f / (Dist + 1.f); // 距離でスコア調整
//            Candidates.Add(Candidate);
//        }
//    }
//
//    // 基地へのルート上の防衛建物優先
//    if (DefenseBase)
//    {
//        TArray<AActor*> FoundStructures;
//        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseStructure::StaticClass(), FoundStructures);
//
//        ADefenseStructure* BlockingStructure = nullptr;
//        float NearestStructureDist = FLT_MAX;
//
//        for (AActor* StructureActor : FoundStructures)
//        {
//            ADefenseStructure* Structure = Cast<ADefenseStructure>(StructureActor);
//            if (!Structure || !IsValid(Structure)) continue;
//
//            FVector ToStructure = Structure->GetActorLocation() - GetActorLocation();
//            FVector ToBase = DefenseBase->GetActorLocation() - GetActorLocation();
//
//            float Dot = FVector::DotProduct(ToStructure.GetSafeNormal(), ToBase.GetSafeNormal());
//            float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
//
//            if (Angle < 60.f) // ±60度以内ならルート上
//            {
//                FHitResult Hit;
//                FCollisionQueryParams Params;
//                Params.AddIgnoredActor(this);
//
//                bool bHit = GetWorld()->LineTraceSingleByChannel(
//                    Hit,
//                    GetActorLocation() + FVector(0, 0, 50.f),
//                    Structure->GetActorLocation() + FVector(0, 0, 50.f),
//                    ECC_Visibility,
//                    Params
//                );
//
//                if (bHit && Hit.GetActor() == Structure)
//                {
//                    float Dist = FVector::Dist(GetActorLocation(), Structure->GetActorLocation());
//                    if (Dist < NearestStructureDist)
//                    {
//                        BlockingStructure = Structure;
//                        NearestStructureDist = Dist;
//                    }
//                }
//            }
//        }
//
//        if (BlockingStructure)
//        {
//            FTargetCandidate Candidate;
//            Candidate.Actor = BlockingStructure;
//            Candidate.Score = 80.f / (NearestStructureDist + 1.f);
//            Candidates.Add(Candidate);
//        }
//        else
//        {
//            // 防衛建物がなければ基地
//            FTargetCandidate Candidate;
//            Candidate.Actor = DefenseBase;
//            Candidate.Score = 50.f; // 基地の基本スコア
//            Candidates.Add(Candidate);
//        }
//    }
//
//    // --- スコア順でソート ---
//    Candidates.Sort([](const FTargetCandidate& A, const FTargetCandidate& B)
//        {
//            return A.Score > B.Score;
//        });
//
//    // --- 最もスコアの高いターゲットを返す ---
//    return Candidates.Num() > 0 ? Candidates[0].Actor : nullptr;
//}
//
//
////AActor* AEnemyCharacterBase::ChooseTarget()
////{
////    // --- プレイヤーと味方キャラを取得 ---
////    TArray<AActor*> PotentialTargets;
////    if (PlayerCharacter)
////    {
////        PotentialTargets.Add(PlayerCharacter);
////    }
////
////    TArray<AActor*> FoundAllies;
////    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);
////    PotentialTargets.Append(FoundAllies);
////
////    // --- 基地を取得 ---
////    TArray<AActor*> FoundBases;
////    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
////    AActor* DefenseBase = (FoundBases.Num() > 0) ? FoundBases[0] : nullptr;
////
////    // --- まず最優先はプレイヤーや味方 ---
////    AActor* NearestAllyOrPlayer = nullptr;
////    float NearestDistance = FLT_MAX;
////
////    for (AActor* Ally : PotentialTargets)
////    {
////        if (!IsValid(Ally)) continue;
////
////        float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
////        if (Dist < NearestDistance)
////        {
////            NearestDistance = Dist;
////            NearestAllyOrPlayer = Ally;
////        }
////    }
////
////    const float PlayerPriorityRange = 2000.f; // プレイヤー/味方優先範囲
////    if (NearestAllyOrPlayer && NearestDistance <= PlayerPriorityRange)
////    {
////        return NearestAllyOrPlayer;
////    }
////
////    // --- 次に基地 ---
////    if (DefenseBase)
////    {
////        // --- 基地への進行ルート上で邪魔な防衛建物がある場合は優先 ---
////        ADefenseStructure* BlockingStructure = nullptr;
////        float NearestStructureDist = FLT_MAX;
////
////        TArray<AActor*> FoundStructures;
////        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseStructure::StaticClass(), FoundStructures);
////
////        for (AActor* StructureActor : FoundStructures)
////        {
////            ADefenseStructure* Structure = Cast<ADefenseStructure>(StructureActor);
////            if (!Structure || !IsValid(Structure)) continue;
////
////            FVector ToStructure = Structure->GetActorLocation() - GetActorLocation();
////            FVector ToBase = DefenseBase->GetActorLocation() - GetActorLocation();
////
////            float Dot = FVector::DotProduct(ToStructure.GetSafeNormal(), ToBase.GetSafeNormal());
////            float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
////
////            // ±60度以内なら基地への進行ルート上とみなす
////            if (Angle < 60.f)
////            {
////                FHitResult Hit;
////                FCollisionQueryParams Params;
////                Params.AddIgnoredActor(this);
////
////                bool bHit = GetWorld()->LineTraceSingleByChannel(
////                    Hit,
////                    GetActorLocation() + FVector(0, 0, 50.f),
////                    Structure->GetActorLocation() + FVector(0, 0, 50.f),
////                    ECC_Visibility, Params);
////
////                if (bHit && Hit.GetActor() == Structure)
////                {
////                    float Dist = FVector::Dist(GetActorLocation(), Structure->GetActorLocation());
////                    if (Dist < NearestStructureDist)
////                    {
////                        BlockingStructure = Structure;
////                        NearestStructureDist = Dist;
////                    }
////                }
////            }
////        }
////
////        // 進行ルート上の防衛建物がある場合は優先
////        if (BlockingStructure)
////        {
////            return BlockingStructure;
////        }
////
////        // 防衛建物がなければ基地をターゲット
////        return DefenseBase;
////    }
////
////    // --- 最後に、防衛用建物（基地やプレイヤーへの進行ルート上でない場合は無視される） ---
////    return nullptr;
////}
//
//float AEnemyCharacterBase::GetEffectiveAttackRange(AActor* Target) const
//{
//    if (Target->IsA(ADefenseBase::StaticClass()))
//    {
//        return 400.f; // 基地には広いレンジ
//    }
//    else if (Target->IsA(AAllyCharacter::StaticClass()))
//    {
//        return 150.f; // 味方ユニットには中距離
//    }
//    else if (Target->IsA(AMyHeroPlayer::StaticClass()))
//    {
//        return 150.f;
//    }
//
//    return AttackRange; // デフォルト
//}
//
//void AEnemyCharacterBase::PerformAttack()
//{
//
//    if (!bCanAttack || bIsDead || !CurrentTarget) return;
//
//    bCanAttack = false;
//
//    UE_LOG(LogTemp, Warning, TEXT("%s attacks %s!"), *GetName(), *CurrentTarget->GetName());
//
//    UGameplayStatics::ApplyDamage(CurrentTarget, AttackDamage, GetController(), this, nullptr);
//
//    if (KillMeFlag)
//    {
//        Die();
//    }
//
//    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
//
//
//    //if (!bCanAttack || !PlayerCharacter || bIsDead) return;
//
//    //AActor* Target = PlayerCharacter;
//    //if (!Target)
//    //{
//    //    // 防衛対象を検索（プレイヤーがいない場合）
//    //    TArray<AActor*> FoundBases;
//    //    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
//    //    if (FoundBases.Num() > 0)
//    //    {
//    //        Target = FoundBases[0];
//    //    }
//    //}
//
//    //if (!Target) return;
//
//    //bCanAttack = false;
//
//    //UE_LOG(LogTemp, Warning, TEXT("Enemy attacks target: %s"), *Target->GetName());
//
//    //UGameplayStatics::ApplyDamage(PlayerCharacter, AttackDamage, GetController(), this, nullptr);
//
//    //GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
//}
//
//void AEnemyCharacterBase::ResetAttack()
//{
//    bCanAttack = true;
//}
//
//float AEnemyCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
//    AController* EventInstigator, AActor* DamageCauser)
//{
//    if (bIsDead) return 0.f;
//
//    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//    CurrentHealth -= ActualDamage;
//    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
//        *GetName(), ActualDamage, CurrentHealth, MaxHealth);
//
//    if (CurrentHealth <= 0.0f)
//    {
//        Die();
//    }
//
//    return ActualDamage;
//}
//
//void AEnemyCharacterBase::Die()
//{
//    if (bIsDead) return;
//    bIsDead = true;
//
//    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died!"), *GetName());
//
//    // TODO: アニメーションやエフェクトを追加したい場合はここで再生
//    // PlayAnimMontage(DeathAnimMontage);
//
//    // 一定時間後に消滅してもよい
//    //SetLifeSpan(2.0f);
//
//    //// GameModeに通知
//    //ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
//    //if (GM)
//    //{
//    //    GM->OnEnemyDestroyed();
//    //}
//
//    // // Spawnerへ直接通知
//    //AEnemySpawnerWave* Spawner = Cast<AEnemySpawnerWave>(
//    //    UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawnerWave::StaticClass())
//    //);
//    //if (Spawner)
//    //{
//    //    Spawner->NotifyEnemyDestroyed(this);
//    //}
//
//    if (!bNotifiedSpawner)
//    {
//        bNotifiedSpawner = true;
//        if (AEnemySpawnerWave* Spawner = Cast<AEnemySpawnerWave>(
//            UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawnerWave::StaticClass())))
//        {
//            Spawner->NotifyEnemyDestroyed(this);
//        }
//    }
//
//    SetLifeSpan(0.2f);
//
//    // すぐに消す場合は
//     Destroy();
//}
