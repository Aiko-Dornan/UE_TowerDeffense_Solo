#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "MyHeroPlayer.h"
#include "AllyCharacter.h"
#include "DefenseBase.h"
#include"DroneCharacter.h"
#include "TD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtr.h"
#include "DefenseStructure.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemySpawnerWave.h"
#include"AmmoDisplayWidget.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/KismetSystemLibrary.h"



//#include "Engine/EngineTypes.h"    // FOverlapResult, FCollisionShape
//#include "Engine/World.h"          // GetWorld()
//
//
//#include "DrawDebugHelpers.h"

// ==================== コンストラクタ ====================

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseDirectMove = false;

    AttackRange = 150.0f;
    AttackCooldown = 2.0f;
   
    //MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = MoveEnemySpeed;

    Mesh = GetMesh();

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    AttackCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollision"));
    AttackCollision->SetupAttachment(GetMesh(), TEXT("RightHandSocket"));
    // ※ ソケット名は実際のものに合わせて

    AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AttackCollision->SetCollisionObjectType(ECC_WorldDynamic);
    AttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    AttackCollision->OnComponentBeginOverlap.AddDynamic(
        this, &AEnemyCharacterBase::OnAttackOverlap);
}

// ==================== BeginPlay ====================

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = MoveEnemySpeed;
    if (!GetController())
    {
        SpawnDefaultController();
    }


    // 通常の初期化
    CurrentHealth = MaxHealth;

    // プレイヤー初期化を追加
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyHeroPlayer::StaticClass(), FoundPlayers);
    if (FoundPlayers.Num() > 0)
    {
        PlayerCharacter = Cast<AMyHeroPlayer>(FoundPlayers[0]);
      
    }

    

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

void AEnemyCharacterBase::TryStartAI()
{
    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: AI initialized successfully."), *GetName());
        UpdateTarget();               // ターゲット選定
        StartMovingToTarget();        // 移動開始
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: AI not ready, retrying..."), *GetName());
        // まだAIがついていなければ再試行
        GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyCharacterBase::TryStartAI);
    }
}


void AEnemyCharacterBase::StartMovingToTarget()
{
    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    {
        if (!IsValid(CurrentTarget))
        {
            UE_LOG(LogTemp, Warning,
                TEXT("%s: StartMovingToTarget failed (no target)"),
                *GetName());
            return;
        }

        // ★ NavMesh 到達不可なら障害物をターゲットにする
        if (!CanReachTarget(CurrentTarget))
        {
            AActor* Blocking = CheckBlockingStructure(CurrentTarget);
            if (IsValid(Blocking))
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("%s: Target %s blocked → switch to %s"),
                    *GetName(),
                    *CurrentTarget->GetName(),
                    *Blocking->GetName());

                PreviousTarget = CurrentTarget;
                CurrentTarget = Blocking;

                if (ADefenseStructure* Struct =
                    Cast<ADefenseStructure>(Blocking))
                {
                    Struct->OnDestroyed.AddDynamic(
                        this,
                        &AEnemyCharacterBase::OnTargetDestroyed
                    );
                }
            }
        }

       /* AI->StopMovement();
        AI->SetFocus(CurrentTarget);

        float Radius = FMath::Max(
            50.0f,
            GetEffectiveAttackRange(CurrentTarget) - 100.0f
        );

        AI->MoveToActor(CurrentTarget, Radius);*/
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("%s: StartMovingToTarget failed (no controller yet)"),
            *GetName());

        GetWorldTimerManager().SetTimerForNextTick(
            this,
            &AEnemyCharacterBase::StartMovingToTarget
        );
    }
}




// ==================== Tick ====================

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead) return;
    if (!IsValid(CurrentTarget)) {
        UE_LOG(LogTemp, Warning, TEXT("{{{%s}}}No Target!!!!!!!!!!!!11!"), *GetName());
        //CurrentTarget = BaseStructure;
        //UE_LOG(LogTemp, Warning, TEXT("{{{%s}}}!!!!!!!!!!!!11!"), *CurrentTarget->GetName());
        return;
    }

    // すでにターゲットを認識しているか
    if (CurrentTarget && !bHasLoggedStuck)
    {
        // 移動速度チェック
        if (GetVelocity().Size() < 1.0f) // 動いていなければ
        {
            bHasLoggedStuck = true; // 一度だけログ
            AEnemyAIController* AICon = Cast<AEnemyAIController>(GetController());
            if (AICon)
            {
                UE_LOG(LogTemp, Error, TEXT("[STUCK AI] %s has Target: %s but is not moving! AIController: %s (Class: %s)"),
                    *GetName(),
                    *CurrentTarget->GetName(),
                    *AICon->GetName(),
                    *AICon->GetClass()->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("[STUCK AI] %s has Target: %s but has NO AIController!"),
                    *GetName(),
                    *CurrentTarget->GetName());
            }
        }
    }

    // ==========================
    // 現在ターゲットへの直線上にバリケードがあるかチェック
    // ==========================
 
    // ==========================
    // 攻撃距離チェック
    // ==========================
    FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
    float Distance = ToTarget.Size();
   const float TestDistance= FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
    float EffectiveAttackRange = GetEffectiveAttackRange(CurrentTarget);
    float DesiredDistance = EffectiveAttackRange/* + 10.0f*/; // ターゲットより10f外側を維持
    FVector Direction = ToTarget.GetSafeNormal();

    // ==========================
    // 移動処理
    // ==========================

   
    if (LockTarget)
    {
        float Radius = FMath::Max(
            50.0f,
            RangeAttack ? GetEffectiveAttackRange(CurrentTarget) - 20.0f : GetEffectiveAttackRange(CurrentTarget) - 200.0f
        );
        FVector TargetLocation = CurrentTarget->GetActorLocation();
        FVector Direction2 = TargetLocation - GetActorLocation();
        Direction2.Z = 0;

        if (Direction2.Size() > Radius)
        {
            Direction2.Normalize();
            AddMovementInput(Direction2, MoveEnemySpeed);
        }
    }
   
    

        // それ以外はAIに任せる
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
           // bUseDirectMove = false;
           // AI->SetFocus(CurrentTarget);
           /* if (!bUseDirectMove)
            {
                AI->SetFocus(CurrentTarget);
            }
            else
            {
                AI->ClearFocus(EAIFocusPriority::Gameplay);
            }*/
            if (RangeAttack)
            {
               // AI->MoveToActor(CurrentTarget, EffectiveAttackRange-200.0f);
            }
            else
            {
                //AI->MoveToActor(CurrentTarget,/* EffectiveAttackRange - 200.0f*/10.0f);
            }


            // MoveToActor は距離が一定以上離れた時のみ再発行
            
           /* const float MoveCommandCooldown = 0.5f;

            if (GetWorld()->GetTimeSeconds() - LastMoveCommandTime > MoveCommandCooldown)
            {
                AI->MoveToActor(CurrentTarget, DesiredDistance);
                LastMoveCommandTime = GetWorld()->GetTimeSeconds();
            }*/
        }
    



    //SetActorRotation(Direction.Rotation());

    // ==========================
    // 攻撃
    // ==========================
    if (Distance <= EffectiveAttackRange && RangeAttack)
    {
       
       
        AllRangeAttack();

        return;
    }
    else if (Distance <= EffectiveAttackRange && KillMeFlag)
    {
        ApplyAreaDamage(AttackDamage,AmountAreaAtacck);
    }
    else if (Distance <= EffectiveAttackRange)
    {
        PerformAttack();
        return;
    }


   
}

bool AEnemyCharacterBase::ShouldAttackBlockingStructure(
    AActor* Target,
    AActor*& OutBlocking
) const
{
    OutBlocking = nullptr;

    if (!IsValid(Target)) return false;

    // すでに障害物を殴っているなら何もしない
    if (Target->IsA(ADefenseStructure::StaticClass()))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("%s already attack;"),*GetName());
        return false;
    }
        

    // 視線上の遮蔽物チェック
    AActor* Blocking = CheckBlockingStructure(Target);
    if (!Blocking) return false;

    const float StraightDist =
        FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    const float NavDist =
        GetNavPathLengthToTarget(Target);

    UE_LOG(LogTemp, Warning,
        TEXT("[PATH CHECK] Straight=%.1f Nav=%.1f Ratio=%.2f"),
        StraightDist,
        NavDist,
        NavDist / StraightDist
    );

    // Nav距離が取れない = 大回り or 到達不能 → 壊す
    if (NavDist == FLT_MAX)
    {
        OutBlocking = Blocking;
        UE_LOG(LogTemp, Warning,
            TEXT("%s will attack;"), *GetName());
        return true;
    }

    // ★ 迂回が許容範囲なら「壊さない」
    if (NavDist <= StraightDist * AllowDetourRatio)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("%s will ukai;"), *GetName());
        return false; // 迂回する
    }

    // ★ それ以上なら「邪魔だから壊す」
    OutBlocking = Blocking;
    UE_LOG(LogTemp, Warning,
        TEXT("%s will kowasu;"), *GetName());
    return true;
}


bool AEnemyCharacterBase::CanReachTarget(AActor* Target) const
{
    if (!Target) return false;

    UNavigationSystemV1* NavSys =
        UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return false;

    UNavigationPath* Path = NavSys->FindPathToActorSynchronously(
        GetWorld(),
        GetActorLocation(),
        Target
    );

    if (!Path || !Path->IsValid())
        return false;

    // Partial = 途中までしか行けない → 障害物越し
    return !Path->IsPartial();
}

bool AEnemyCharacterBase::ShouldSwitchToBlocking(AActor* Target, AActor*& OutBlocking) const
{
    OutBlocking = nullptr;

    if (!IsValid(Target)) return false;
    if (Target->IsA(ADefenseStructure::StaticClass())) return false;

   

    AActor* Blocking = CheckBlockingStructure(Target);
    if (!Blocking) return false;

    const float StraightDist =
        FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    const float NavDist =
        GetNavPathLengthToTarget(Target);

    // ★ 切り替え条件
    const bool bShouldSwitch =
        !CanReachTarget(Target) ||
        NavDist == FLT_MAX ||
        NavDist > StraightDist * AllowDetourRatio;

    if (bShouldSwitch)
    {
        OutBlocking = Blocking;
        return true;
    }

    return false;
}


float AEnemyCharacterBase::GetNavPathLengthToTarget(AActor* Target) const
{
    if (!Target) return FLT_MAX;

    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys) return FLT_MAX;

    UNavigationPath* Path = NavSys->FindPathToActorSynchronously(
        GetWorld(),
        GetActorLocation(),
        Target
    );

    if (!Path || !Path->IsValid() || Path->PathPoints.Num() < 2)
        return FLT_MAX;

    float TotalLength = 0.f;
    for (int32 i = 1; i < Path->PathPoints.Num(); ++i)
    {
        TotalLength += FVector::Dist(
            Path->PathPoints[i - 1],
            Path->PathPoints[i]
        );
    }

    return TotalLength;
}


AActor* AEnemyCharacterBase::CheckBlockingStructure(AActor* MainTarget)const
{
    if (!IsValid(MainTarget)) return nullptr;

    FVector Start = GetActorLocation() + FVector(0, 0, 10.f);
    FVector End = MainTarget->GetActorLocation() + FVector(0, 0, 10.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

   /* bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_WorldStatic,
        Params
    );*/

    FCollisionObjectQueryParams ObjParams;
    ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
    ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic); // DefenseStructure 用

    bool bHit = GetWorld()->LineTraceSingleByObjectType(
        Hit,
        Start,
        End,
        ObjParams,
        Params
    );

   /* DrawDebugLine(GetWorld(), Start, End,
        bHit ? FColor::Red : FColor::Green,
        false, 0.1f, 0, 1.5f);*/

    if (bHit)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Trace Hit: %s (%s)by%s"),
            *Hit.GetActor()->GetName(),
            *Hit.GetActor()->GetClass()->GetName(),
            *this->GetName()
        );
    }

    if (!bHit || !Hit.GetActor())
        return nullptr;

    if (ADefenseStructure* HitStructure = Cast<ADefenseStructure>(Hit.GetActor()))
    {
        float DistToHit = FVector::Dist(GetActorLocation(), HitStructure->GetActorLocation());
        float DistToTarget = FVector::Dist(GetActorLocation(), MainTarget->GetActorLocation());

        // ★ ターゲットより手前の障害物のみ有効
        if (DistToHit < DistToTarget)
        {
           
            return HitStructure;
        }
    }

    return nullptr;
}




void AEnemyCharacterBase::UpdateTarget()
{
    if (bIsDead) return;

    if (!IsValid(CurrentTarget))
    {
        LockTarget = false;
    }

    if (!LockTarget)
    {
        //UE_LOG(LogTemp, Warning, TEXT("%s Locked Target!"),
        //    *GetName());
        CurrentTarget = ChooseTargetBP_Implementation();
        BlockTarget = nullptr;

        if (!IsValid(CurrentTarget))
        {
            CurrentTarget = BaseStructure;
            if (!IsValid(CurrentTarget)) return;
        }

        AActor* Blocking = nullptr;
        if (ShouldAttackBlockingStructure(CurrentTarget, Blocking))
        {
            PreviousTarget = CurrentTarget;
            CurrentTarget = Blocking;
            LockTarget = true;
            BlockTarget = Blocking;
            if (ADefenseStructure* Struct = Cast<ADefenseStructure>(Blocking))
            {
                Struct->OnDestroyed.AddDynamic(
                    this,
                    &AEnemyCharacterBase::OnTargetDestroyed
                );
            }
            //return;
        }
        else
        {
            LockTarget = false;
        }

      

        //if (NewTarget == CurrentTarget) 
        { 
            if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
            {
                //GetCharacterMovement()->DisableMovement();
                AI->ClearFocus(EAIFocusPriority::Gameplay);
                AI->SetFocus(CurrentTarget);

                float Radius = FMath::Max(
                    50.0f,
                    RangeAttack ? GetEffectiveAttackRange(CurrentTarget) - 20.0f : GetEffectiveAttackRange(CurrentTarget) - 200.0f
                );
                UE_LOG(LogTemp, Warning, TEXT("%s is watching %s?"), *GetName(), *CurrentTarget->GetName());
                AI->MoveToActor(CurrentTarget, Radius);
                MoveORIdle();

            }
        }
       /* else
        {
            CurrentTarget = NewTarget;

            if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
            {
                AI->StopMovement();
                AI->ClearFocus(EAIFocusPriority::Gameplay);
                AI->SetFocus(CurrentTarget);

                float Radius = FMath::Max(
                    50.0f,
                    RangeAttack ? GetEffectiveAttackRange(CurrentTarget) - 20.0f : GetEffectiveAttackRange(CurrentTarget) - 200.0f
                );
                UE_LOG(LogTemp, Warning, TEXT("%s is funking %s?"), *GetName(), *CurrentTarget->GetName());
                AI->MoveToActor(CurrentTarget, Radius);
                MoveORIdle();

            }
        }*/

        
       
    }
    else /*if (LockTarget)*/
    {
      /* CurrentTarget = ChooseTargetBP_Implementation();
        LockTarget = false;
        BlockTarget = nullptr;
        PreviousTarget = nullptr;*/
       // UE_LOG(LogTemp, Warning, TEXT("%s is watching %s?"), *GetName(), *CurrentTarget->GetName());
 
        
        AActor* ThirdTarget = ChooseTargetBP_Implementation();

        AActor* Blocking = nullptr;
        if (ShouldAttackBlockingStructure(ThirdTarget, Blocking)&& ShouldAttackBlockingStructure(PreviousTarget, Blocking))
        {
           // PreviousTarget = CurrentTarget;
            //CurrentTarget = Blocking;
            LockTarget = true;
            //BlockTarget = Blocking;
           /* if (ADefenseStructure* Struct = Cast<ADefenseStructure>(Blocking))
            {
                Struct->OnDestroyed.AddDynamic(
                    this,
                    &AEnemyCharacterBase::OnTargetDestroyed
                );
            }*/
        }
        else
        {
            LockTarget = false;
            UpdateTarget();
           // return;
        }

        

        //CurrentTarget = ChooseTarget_DefenseStructure();

        UE_LOG(LogTemp, Warning, TEXT("%s is targeting %s !"),
            *GetName(), *CurrentTarget->GetName());

        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            //LockTarget = false;
            //BlockStructure = nullptr;
            //AI->StopMovement();
            AI->ClearFocus(EAIFocusPriority::Gameplay);
            AI->SetFocus(CurrentTarget);

           /* float Radius = FMath::Max(
                50.0f,
                RangeAttack ? GetEffectiveAttackRange(CurrentTarget) - 20.0f : GetEffectiveAttackRange(CurrentTarget) - 200.0f
            );

            AI->MoveToActor(CurrentTarget, Radius);*/

           

            MoveORIdle();
            
        }

        //return;

       
    }

    //if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    //{
    //    //GetCharacterMovement()->DisableMovement();
    //    AI->ClearFocus(EAIFocusPriority::Gameplay);
    //    AI->SetFocus(CurrentTarget);

    //    float Radius = FMath::Max(
    //        50.0f,
    //        RangeAttack ? GetEffectiveAttackRange(CurrentTarget) - 20.0f : GetEffectiveAttackRange(CurrentTarget) - 200.0f
    //    );
    //    UE_LOG(LogTemp, Warning, TEXT("%s is watching %s(%s)?"), *GetName(), *CurrentTarget->GetName(),*CurrentTarget->GetActorLocation().ToString());

    //  /*  FVector TargetLocation = CurrentTarget->GetActorLocation();
    //    FAIMoveRequest MoveRequest;
    //    MoveRequest.SetGoalLocation(TargetLocation);
    //    MoveRequest.SetAcceptanceRadius(Radius);
    //    

    //    FVector Direction = TargetLocation - GetActorLocation();
    //    Direction.Z = 0;

    //    if (Direction.Size() > Radius)
    //    {
    //        Direction.Normalize();
    //        AddMovementInput(Direction, MoveEnemySpeed);
    //    }*/
    //    //AI->MoveTo(MoveRequest);
    //   // AI->MoveToLocation(TargetLocation, Radius);
    //    //AI->MoveToActor(CurrentTarget, Radius);
    //    MoveORIdle();

    //}

   /* if (!IsValid(CurrentTarget)||NewTarget!=Blocking)
    {
        LockTarget = false;
    }

    if (LockTarget)
    {
        return;
    }*/

   


}

void AEnemyCharacterBase::MoveORIdle()
{
    UE_LOG(LogTemp, Warning, TEXT("%s is %f! !"),
        *GetName(), GetVelocity().Size());

    if (GetVelocity().Size() < 1.0f ||GetCharacterMovement()->IsMovingOnGround() /*&&
        !GetController()->IsFollowingAPath()*/)
    {
        if (MoveAnim)
        {
            if (CurrentAnimType == EEnemyAnimType::Move)
            {
                UE_LOG(LogTemp, Warning, TEXT("%s is Dash! !"),
                    *GetName());
               return;
            }
            else
            {
                PlayAnimation(EEnemyAnimType::Move, true);
               
                //UE_LOG(LogTemp, Warning, TEXT("RUNNING!"));
            }
        }
    }
    else
    {
        if (IdleAnim)
        {
            if (CurrentAnimType == EEnemyAnimType::Idle)
            {
               
                return;
            }
            else
            {
                PlayAnimation(EEnemyAnimType::Idle, true);
              
                //UE_LOG(LogTemp, Warning, TEXT("IDLING!"));
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

       /* if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            if (IsValid(CurrentTarget) && CurrentTarget != BaseStructure)
            {
                AI->SetFocus(CurrentTarget);
                float Radius = FMath::Max(50.0f, GetEffectiveAttackRange(CurrentTarget) - 100.0f);
                AI->MoveToActor(CurrentTarget, Radius);
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

    if (ADroneCharacter* Drone = Cast<ADroneCharacter>(OtherActor))
    {
        CurrentTarget = Drone;
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
        LockTarget = false;
        bUseDirectMove = false; // ←これを追加
        //CurrentTarget = nullptr;
        /*CurrentTarget = ChooseTarget_Default();*/
        //CurrentTarget = ChooseTargetBP();

        if (!IsValid(CurrentTarget))
        {
            CurrentTarget = BaseStructure;
        }

        // 元ターゲット（プレイヤー・基地など）に戻る
        if (IsValid(PreviousTarget) && CanReachTarget(PreviousTarget))
        {
            CurrentTarget = PreviousTarget;
            PreviousTarget = nullptr;
        }
        else
        {
            UpdateTarget();
            return;
        }

        // --- フォーカスをリセット ---
        if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
        {
            AI->ClearFocus(EAIFocusPriority::Gameplay);
            AI->StopMovement();

            if (IsValid(CurrentTarget)/*&&!law_inteli_flag*/)
            {
                AI->SetFocus(CurrentTarget);
                float Radius = FMath::Max(50.0f, GetEffectiveAttackRange(CurrentTarget) - 100.0f);
                AI->MoveToActor(CurrentTarget, Radius);
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
    if (AMyHeroPlayer* Hero = Cast<AMyHeroPlayer>(PlayerCharacter))
    {
        if (!Hero->bIsDead)
        {
            const float Dist = FVector::Dist(GetActorLocation(), Hero->GetActorLocation());
            if (Dist < MaxConsiderRange)
            {
                FTargetCandidate Candidate;
                Candidate.Actor = Hero;
                Candidate.Score =
                    PlayerPriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
                Candidates.Add(Candidate);
            }
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

    //---ドローン---

    TArray<AActor*> FoundDrones;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADroneCharacter::StaticClass(), FoundDrones);
    for (AActor* Drone : FoundDrones)
    {
        if (!IsValid(Drone)) continue;
        const float Dist = FVector::Dist(GetActorLocation(), Drone->GetActorLocation());
        if (Dist < MaxConsiderRange)
        {
            FTargetCandidate Candidate;
            Candidate.Actor = Drone;
            Candidate.Score = DronePriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
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

//
AActor* AEnemyCharacterBase::ChooseTarget_DefenseStructure()
{
    // 停止中なら認識範囲を2倍に
    const float MaxConsiderRangeBase = bIsRecognitionExtended ? ExtendedConsiderRange : DefaultConsiderRange;
    const float MaxConsiderRange = 18000.0f;

    const float BlockCheckAngle = 345.f;

    TArray<FTargetCandidate> Candidates;

     // --- バリケード ---
    TArray<AActor*> FoundStructures;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseStructure::StaticClass(), FoundStructures);
    for (AActor* Structure : FoundStructures)
    {
        if (!IsValid(Structure)) continue;
        const float Dist = FVector::Dist(GetActorLocation(), Structure->GetActorLocation());
        if (Dist < MaxConsiderRange)
        {
            FTargetCandidate Candidate;
            Candidate.Actor = Structure;
            Candidate.Score = StructurePriority * FMath::Clamp(1.f - (Dist / MaxConsiderRange), 0.f, 1.f);
            Candidates.Add(Candidate);
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

// ==================== 攻撃処理 ====================

void AEnemyCharacterBase::PerformAttack()//近距離
{
    if (!bCanAttack || bIsDead || !IsValid(CurrentTarget)) return;

   

    UE_LOG(LogTemp, Warning, TEXT("%s attacks %s!"),
        *GetName(), *CurrentTarget->GetName());

  

    if (AttackAnim)
    {
        GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed / 10;
        PlayAnimation(EEnemyAnimType::Attack, false);
        /*GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this,
            &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);*/
    }
  
  

    //UGameplayStatics::ApplyDamage(CurrentTarget, AttackDamage, GetController(), this, nullptr);

}

void AEnemyCharacterBase::AllRangeAttack()
{
  /*  if (!bCanAttack) UE_LOG(LogTemp, Error, TEXT("%s → bCanAttack == false"), *GetName());
    if (bIsDead) UE_LOG(LogTemp, Error, TEXT("%s → bIsDead == true"), *GetName());
    if (!CurrentTarget) UE_LOG(LogTemp, Error, TEXT("%s → CurrentTarget == NULL"), *GetName());*/

    if (!bCanAttack || bIsDead || !CurrentTarget)
    {
        //UE_LOG(LogTemp, Error, TEXT("%s → Attack aborted"), *GetName());
        return;
    }




    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is NULL!!"));
        return;
    }

    if (RangeAttackAnim)
    {
        PlayAnimation(EEnemyAnimType::RangeAttack, false);
    }

    

    // 敵 Mesh を取得
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Error, TEXT("Enemy has no SkeletalMesh!"));
        return;
    }

    FVector MuzzleLocation = MeshComp->GetSocketLocation("EnemyFireSocket");
    FRotator MuzzleRotation = MeshComp->GetSocketRotation("EnemyFireSocket");

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AMyGrenadeProjectileActor* Grenade = GetWorld()->SpawnActor<AMyGrenadeProjectileActor>(
        ProjectileClass,
        MuzzleLocation,
        MuzzleRotation,
        SpawnParams
    );

    if (Grenade)
    {
        Grenade->Damage = AttackDamage;
        Grenade->AmountAreaAtacck = AmountAreaAtacck;
        Grenade->TargetLocation = CurrentTarget->GetActorLocation();
        Grenade->CalculateLaunchVelocity(); // ここで velocity を計算
        UE_LOG(LogTemp, Warning, TEXT("Projectile Spawned!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Projectile Spawn FAILED!!"));
    }
    bCanAttack = false;
    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this,
        &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);

}

// ==================== 範囲攻撃（全対象に即時ダメージ） ====================
void AEnemyCharacterBase::ApplyAreaDamage(float DamageAmount, float Radius)
{
    if (bIsDead) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FVector Center = GetActorLocation();   // プレイヤーの位置
    float Radius2 = Radius;           // 検索半径（例：500.f）

    // 取得結果を入れる配列
    TArray<AActor*> FoundActors;

    // 取得対象の ObjectType（例：WorldDynamic）
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    // 無視する Actor（自分）
    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);

    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        World,
        Center,
        Radius,
        ObjectTypes,
        AActor::StaticClass(),  // 取得したいクラス
        IgnoreActors,
        FoundActors
    );

    if (bHit)
    {
        for (AActor* Actor : FoundActors)
        {
            // --- 範囲ダメージを与える対象 ---
            if (Actor->IsA(AMyHeroPlayer::StaticClass()) ||
                Actor->IsA(AAllyCharacter::StaticClass()) ||
                Actor->IsA(ADroneCharacter::StaticClass()) ||
                Actor->IsA(ADefenseBase::StaticClass()) ||
                Actor->IsA(ADefenseStructure::StaticClass()))
            {
                UGameplayStatics::ApplyDamage(
                    Actor,
                    DamageAmount,
                    GetController(),
                    this,
                    nullptr
                );

                UE_LOG(LogTemp, Warning, TEXT("%s - AreaDamage applied to %s巻き込みました!!"),
                    *GetName(), *Actor->GetName());
            }

            UE_LOG(LogTemp, Warning, TEXT("Actor in range: %s"), *Actor->GetName());
        }
    }

   

    // クールダウン開始
    bCanAttack = false;

    GetWorldTimerManager().SetTimer(
        AttackCooldownTimerHandle,
        this,
        &AEnemyCharacterBase::ResetAttack,
        AttackCooldown,
        false
    );

    Die();
}




void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
    bIsAnimationLocked = false;
    GetCharacterMovement()->MaxWalkSpeed = MoveEnemySpeed;

    FVector ToTarget = CurrentTarget->GetActorLocation() - GetActorLocation();
    float Distance = ToTarget.Size();
    UE_LOG(LogTemp, Warning, TEXT("%s Range %f!"),*GetName(),Distance);
    if (Distance< GetEffectiveAttackRange(CurrentTarget))
    {
        return;
    }
    else
    {
        MoveORIdle();
    }
    
}

void AEnemyCharacterBase::OnAttackHit()
{
    if (bIsDead || !IsValid(CurrentTarget)) return;

    UE_LOG(LogTemp, Warning, TEXT("Attack Hit!"));

    UGameplayStatics::ApplyDamage(
        CurrentTarget,
        AttackDamage,
        GetController(),
        this,
        nullptr
    );
}

void AEnemyCharacterBase::OnAttackOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
  

    if (!OtherActor)
    {
        return;
    }
    if (OtherActor == this) { 
        /*return;*/
        //UE_LOG(LogTemp, Warning, TEXT("Melee Hit: %s:MyOwn"), *OtherActor->GetName());
       // OtherActor = CurrentTarget;
    }
    UE_LOG(LogTemp, Warning, TEXT("Melee Hit: %s:3"), *OtherActor->GetName());
    //if (HitActors.Contains(OtherActor)) return;
    /*UE_LOG(LogTemp, Warning, TEXT("Melee Hit: %s:4"), *OtherActor->GetName());*/
    // 対象チェック
    if (OtherActor->IsA(AMyHeroPlayer::StaticClass())  ||
        OtherActor->IsA(AAllyCharacter::StaticClass()) ||
        OtherActor->IsA(ADroneCharacter::StaticClass())||
        OtherActor->IsA(ADefenseBase::StaticClass())   ||
        OtherActor->IsA(ADefenseStructure::StaticClass()))
    {
        //HitActors.Add(OtherActor);

        UGameplayStatics::ApplyDamage(
            OtherActor,
            AttackDamage,
            GetController(),
            this,
            nullptr
        );
        bCanAttack = false;
        UE_LOG(LogTemp, Warning, TEXT("%s Melee Hit: %s"), *GetName(),*CurrentTarget->GetName());
        return;
    }
    /*GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this,
        &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);*/
    //bCanAttack = false;
   
}



// ==================== 被ダメージ処理 ====================

float AEnemyCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f;

    bIsAnimationLocked = false;
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHealth -= ActualDamage;
    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
        *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    if (DamageAnim)
    {
        
        PlayAnimation(EEnemyAnimType::Damage, false);
    }

    GetCharacterMovement()->MaxWalkSpeed= GetCharacterMovement()->MaxWalkSpeed/10;

    //LockTarget = false;
    
   

    if (CurrentHealth <= 0.0f)
    {
        bIsAnimationLocked = false;
        Die();
    }
    else
    {
        GetWorldTimerManager().SetTimer(
            LockReleaseHandle,
            this,
            &AEnemyCharacterBase::LockRelease,
           MahiTime,
            false
        );
    }
   
   
   

    return ActualDamage;
}

// ==================== 死亡処理 ====================

void AEnemyCharacterBase::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died!"), *GetName());

    

   /* TArray<AActor*> FoundPlayer;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyHeroPlayer::StaticClass(), FoundPlayer);
    MHP = Cast<AMyHeroPlayer>(FoundPlayer[0]);

    if (MHP != nullptr && MHP->AmmoWidget != nullptr)
    {
        MHP->AmmoWidget->UpdateDroneText(5);
    }*/

    if (!bNotifiedSpawner)
    {
        bNotifiedSpawner = true;
        if (AEnemySpawnerWave* Spawner = Cast<AEnemySpawnerWave>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawnerWave::StaticClass())))
        {
            Spawner->NotifyEnemyDestroyed(this);
        }
    }

    if (AEnemyAIController* AI = Cast<AEnemyAIController>(GetController()))
    {
        AI->StopMovement();
        AI->ClearFocus(EAIFocusPriority::Gameplay);
    }

    //MoveEnemySpeed = 0.0f;

    if (DeadAnim && CurrentHealth <= 0)
    {
        /*  Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);*/
        PlayAnimation(EEnemyAnimType::Dead, false); // Dead
       
        GetWorldTimerManager().SetTimer(
            LockReleaseHandle,
            this,
            &AEnemyCharacterBase::LockRelease,
           VanishTime,
            false
        );

    }
    else
    {
        PlayNiagaraEffect();

        Destroy();
    }
   

    

   
}

// ==================== 攻撃レンジ設定 ====================

float AEnemyCharacterBase::GetEffectiveAttackRange(AActor* Target) const
{
    if (Target->IsA(ADefenseBase::StaticClass()))
    {
        return BaseAttackRange; // 基地には広いレンジ
    }
    else if (Target->IsA(ADefenseStructure::StaticClass()))
    {
        return BarricadeAttackRange; // 建物はやや広め
    }
    else if (Target->IsA(AAllyCharacter::StaticClass()) || Target->IsA(AMyHeroPlayer::StaticClass()))
    {
        return PlayerAllyAttackRange;
    }
    else if (Target->IsA(ADroneCharacter::StaticClass()) || Target->IsA(ADroneCharacter::StaticClass()))
    {
        return DroneAttackRange;
    }

    return AttackRange;
}

void AEnemyCharacterBase::PlayNiagaraEffect()
{
    if (NiagaraEffect)
    {
        // ワールド上のこのアクタの位置にパーティクルをスポーン
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            NiagaraEffect,
            GetActorLocation(),
            FRotator::ZeroRotator,
            FVector(4.0f),  // スケール
            true,           // 自動破棄
            true,           // 自動アクティブ
            ENCPoolMethod::None,
            true
        );
    }
}

//void AEnemyCharacterBase::SetEnemyState(EEnemyState NewState)
//{
//    if (EnemyState == NewState) return;
//
//    EnemyState = NewState;
//
//    // AnimBP に即時反映させたい場合
//    if (USkeletalMeshComponent* MeshComp = GetMesh())
//    {
//        if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
//        {
//            // AnimBP 側は EnemyState を直接参照するだけでOK
//        }
//    }
//}

UAnimationAsset* AEnemyCharacterBase::GetAnimByType(EEnemyAnimType Type) const
{
    switch (Type)
    {
    case EEnemyAnimType::Idle:   return IdleAnim;
    case EEnemyAnimType::Move:   return MoveAnim;
    case EEnemyAnimType::Attack: return AttackAnim;
    case EEnemyAnimType::RangeAttack: return RangeAttackAnim;
    case EEnemyAnimType::Dead:   return DeadAnim;
    case EEnemyAnimType::Damage:   return DamageAnim;
    default:                     return nullptr;

    
    }
}

void AEnemyCharacterBase::PlayAnimation(EEnemyAnimType NewType, bool bLoop)
{
   
    

    // ロック中は無視
    if (bIsAnimationLocked)
    {
        return;
    }

    UAnimationAsset* Anim = GetAnimByType(NewType);
    if (!Anim) { 
        UE_LOG(LogTemp, Warning, TEXT("No Anime"));
        return; }

    USkeletalMeshComponent* USMesh = GetMesh();
    if (!USMesh) return;

    USMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    USMesh->PlayAnimation(Anim, bLoop);

    CurrentAnimType = NewType;

    // ロック対象ならロック
    if (IsLockedAnim(NewType))
    {
        bIsAnimationLocked = true;
    }

}

bool AEnemyCharacterBase::IsLockedAnim(EEnemyAnimType Type) const
{
    return Type == EEnemyAnimType::Attack
        || Type == EEnemyAnimType::Dead
        || Type==EEnemyAnimType::Damage;
}

void AEnemyCharacterBase::LockRelease()
{

    bIsAnimationLocked = false;
    
    if (bIsDead)
    {
        PlayNiagaraEffect();

        Destroy();
    }
    else
    {
        LockTarget = false;
        GetCharacterMovement()->MaxWalkSpeed = MoveEnemySpeed;
        UpdateTarget();
        
        UE_LOG(LogTemp, Warning, TEXT("Damage Anime end"));
    }

}
