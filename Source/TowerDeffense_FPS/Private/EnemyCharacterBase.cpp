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
    bUseDirectMove = false;

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

        if(!GetController())
    {
        SpawnDefaultController();
    }


    // 通常の初期化
    CurrentHealth = MaxHealth;

    // ターゲット設定
    if (!IsValid(BaseStructure))
    {
        TArray<AActor*> FoundBases;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
        if (FoundBases.Num() > 0)
        {
            BaseStructure = Cast<ADefenseBase>(FoundBases[0]);
            
        }
    }

    CurrentTarget = BaseStructure;
    bUseDirectMove = false; // 最初はAIに任せる

    // --- ターゲットが設定されていれば1フレーム遅延で MoveTo 実行 ---
    if (IsValid(CurrentTarget))
    {
        GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyCharacterBase::StartMovingToTarget);
    }
    //GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyCharacterBase::StartMovingToTarget);

    // ターゲット更新タイマー
    GetWorldTimerManager().SetTimer(
        TargetUpdateTimerHandle,
        this,
        &AEnemyCharacterBase::UpdateTarget,
        TargetUpdateInterval,
        true
    );



}

void AEnemyCharacterBase::StartMovingToTarget()
{
    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    {
        if (IsValid(CurrentTarget))
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: StartMovingToTarget -> %s"), *GetName(), *CurrentTarget->GetName());
            AI->SetFocus(CurrentTarget);
            AI->MoveToActor(CurrentTarget, GetEffectiveAttackRange(CurrentTarget));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: StartMovingToTarget failed (no target)"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: StartMovingToTarget failed (no controller yet)"), *GetName());
        // Controllerがまだなければ再試行
        GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyCharacterBase::StartMovingToTarget);
    }
}



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
        bUseDirectMove = false;

   
        if (BlockingStructure != CurrentTarget && BlockingStructure)
        {
            PreviousTarget = CurrentTarget;
            CurrentTarget = BlockingStructure;

            // 建物破壊通知を受け取る
            if (ADefenseStructure* Struct = Cast<ADefenseStructure>(BlockingStructure))
            {
                Struct->OnDestroyed.AddDynamic(this, &AEnemyCharacterBase::OnTargetDestroyed);
            }
            bUseDirectMove = true; // バリケード直進フラグ
        }

       
    

    // ==========================
    // 攻撃距離チェック
    // ==========================
    FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
    float Distance = ToTarget.Size();
    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
    float DesiredDistance = EffectiveAttackRange/* + 10.0f*/; // ターゲットより10f外側を維持
    FVector Direction = ToTarget.GetSafeNormal();

    // ==========================
    // 移動処理
    // ==========================

    if (bUseDirectMove)
    {
        // バリケードに対しては手動で移動
        if (Distance > DesiredDistance)
        {
            /*FVector NewLocation = GetActorLocation() + Direction * GetCharacterMovement()->MaxWalkSpeed * DeltaTime;
            SetActorLocation(NewLocation, true);*/

            if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
            {
                AI->StopMovement();
                AI->ClearFocus(EAIFocusPriority::Gameplay);
            }

            bUseDirectMove = true;

            FVector Dir = (BlockingStructure->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            AddMovementInput(Dir, 5.0f); // これならNavMeshや物理と自然に共存できる

        }
    }
    else
    {
        // それ以外はAIに任せる
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            bUseDirectMove = false;

            AI->SetFocus(CurrentTarget);
            AI->MoveToActor(CurrentTarget, EffectiveAttackRange-200.0f);

            // MoveToActor は距離が一定以上離れた時のみ再発行
             float LastMoveCommandTime = 0.0f;
            const float MoveCommandCooldown = 0.5f;

            if (GetWorld()->GetTimeSeconds() - LastMoveCommandTime > MoveCommandCooldown)
            {
               
                LastMoveCommandTime = GetWorld()->GetTimeSeconds();
            }
        }
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

   /* if (CurrentTarget && !CurrentTarget->IsValidLowLevel())
    {
        CurrentTarget = nullptr;
    }*/

    // ==========================
    // AI 移動はバリケード以外のみ
    // ==========================
    /*if (!bUseDirectMove)
    {
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            AI->SetFocus(CurrentTarget);
            AI->MoveToActor(CurrentTarget,5.0f);
        }
    }*/
}






AActor* AEnemyCharacterBase::CheckBlockingStructure(AActor* MainTarget)
{
    if (!IsValid(MainTarget)) return nullptr;

    // すでにターゲットがバリケードならチェック不要
    if (MainTarget->IsA(ADefenseStructure::StaticClass()))
        return nullptr;

    UCapsuleComponent* Capsule = GetCapsuleComponent();
    float HalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.f;

    FVector Start = GetActorLocation() - FVector(0, 0, HalfHeight - 20.f);
    FVector End = MainTarget->GetActorLocation() + FVector(0, 0, 20.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility/*ECC_GameTraceChannel1*/,
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

            if (IsValid(CurrentTarget)/*&&!law_inteli_flag*/)
            {
                AI->SetFocus(CurrentTarget);
                AI->MoveToActor(CurrentTarget, GetEffectiveAttackRange(CurrentTarget) - 200.0f);
            }
        }
    }
}


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
        //CurrentTarget = nullptr;
        CurrentTarget = ChooseTarget_Default();

        if (!IsValid(CurrentTarget))
        {
            CurrentTarget = BaseStructure;
        }

        //// 元ターゲット（プレイヤー・基地など）に戻る
        //if (IsValid(PreviousTarget))
        //{
        //    CurrentTarget = PreviousTarget;
        //    PreviousTarget = nullptr;
        //}
        //else
        //{
        //    CurrentTarget = ChooseTarget_Default();
        //}

        // --- フォーカスをリセット ---
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            AI->ClearFocus(EAIFocusPriority::Gameplay);
            AI->StopMovement();

            if (IsValid(CurrentTarget)/*&&!law_inteli_flag*/)
            {
                AI->SetFocus(CurrentTarget);
                AI->MoveToActor(CurrentTarget, GetEffectiveAttackRange(CurrentTarget)-200.0f);
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
                Candidate.Score = BasePriority * FMath::Clamp(1.f - (Dist / (MaxConsiderRangeBase)), 0.f, 1.f);
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

                   
                    
                }
                
               

            }
         
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
    if (!IsValid(BaseStructure))
    {
        TArray<AActor*> FoundBases;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
        if (FoundBases.Num() > 0)
        {
            BaseStructure = Cast<ADefenseBase>(FoundBases[0]);
            bUseDirectMove = true;
        }
    }

    return IsValid(BaseStructure) ? BaseStructure : nullptr;

    //return Candidates[0].Actor;
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
        return 600.f; // 基地には広いレンジ
    }
    else if (Target->IsA(ADefenseStructure::StaticClass()))
    {
        return 300.f; // 建物はやや広め
    }
    else if (Target->IsA(AAllyCharacter::StaticClass()) || Target->IsA(AMyHeroPlayer::StaticClass()))
    {
        return 200.f;
    }

    return AttackRange;
}


