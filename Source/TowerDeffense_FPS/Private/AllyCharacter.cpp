#include "AllyCharacter.h"
#include "AAllyAIController.h"
#include "WeaponBase.h"
#include "EnemyCharacterBase.h"
#include "AIController.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include"ItemBase.h"
#include"AmmoDisplayWidget.h"


AAllyCharacter::AAllyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAllyAIController::StaticClass();

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void AAllyCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    InitialPosition = GetActorLocation();

    if (WeaponClass)
    {
        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
        if (EquippedWeapon)
        {
            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
            EquippedWeapon->SetOwner(this);
        }
    }

    TArray<AActor*> FoundPlayer;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyHeroPlayer::StaticClass(), FoundPlayer);
    MHP = Cast<AMyHeroPlayer>(FoundPlayer[0]);
    ReGetawayTime = GetawayTime;
    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
}

void AAllyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //----------------------------------------
    // ■ ターゲットロック維持処理
    //----------------------------------------
    if (TargetEnemy && IsValid(TargetEnemy))
    {
        // 経過時間加算
        TargetLockedElapsed += DeltaTime;

        float DistToTarget = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

        // 一定距離より離れた or 時間が経過したらロック解除
        if (DistToTarget > TargetLoseDistance || TargetLockedElapsed > TargetLockTime)
        {
            TargetEnemy = nullptr;
        }
    }

    //----------------------------------------
    // ■ ターゲットがいない場合のみ検索
    //----------------------------------------
    if (!TargetEnemy || !IsValid(TargetEnemy))
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        FindNearestEnemy();

        // 見つからなければ初期位置へ戻る
        if (!TargetEnemy)
        {
            MoveBackToInitialPosition();
            return;
        }
        else
        {
            // 新しいターゲットをロック開始
            TargetLockedElapsed = 0.f;
        }
    }

    //----------------------------------------
    // ■ 敵がいる時の行動
    //----------------------------------------

    if (!bIsGetaway)
    {
        FaceTarget(TargetEnemy);
    }

    const float DistanceToEnemy = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

    // 敵が検知範囲外に出たらターゲット解除 → 初期位置へ
    if (DistanceToEnemy > EnemyDetectRange)
    {
        TargetEnemy = nullptr;
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        MoveBackToInitialPosition();
        return;
    }

    //// 接近しすぎたら後退
    //if (DistanceToEnemy < MaintainDistance)
    //{
    //    MoveAwayFromEnemy(TargetEnemy);
    //}
    //else
    //{
    //    StopMovement();
    //}

    // ================================
// ■ 逃走中は最優先で距離を取る
// ================================
    if (bIsGetaway)
    {
        if (!bIsGetawayMoving)
        {
            MoveAwayFromEnemy(TargetEnemy);
            bIsGetawayMoving = true;
        }
        return;
    }

    // ================================
    // ■ 通常時の距離制御
    // ================================
    if (DistanceToEnemy < MaintainDistance)
    {
        MoveAwayFromEnemy(TargetEnemy);
    }
    else
    {
        StopMovement();
    }


    // 射撃範囲内なら射撃開始
    if (DistanceToEnemy <= FireRange)
    {
        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
        {
            GetWorldTimerManager().SetTimer(FireTimerHandle, this,
                &AAllyCharacter::HandleFire, FireInterval, true);
        }
    }
    else
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
    }

   /* if (!bIsAnimationLocked)
    {
        EAllyAnimType NewType = GetMoveAnimByDirection();

        if (NewType != CurrentAnimType)
        {
            PlayAnimation(NewType, true);
        }
    }*/

    AnimChangeElapsed += DeltaTime;

    if (AnimChangeElapsed >= AnimChangeCooldown && !bIsAnimationLocked)
    {
        EAllyAnimType NewType = GetMoveAnimByDirection();

        if (NewType != CurrentAnimType)
        {
            PlayAnimation(NewType, true);
            AnimChangeElapsed = 0.f;
        }
       
    }

}


EAllyAnimType AAllyCharacter::GetMoveAnimByDirection() const
{
    FVector Velocity = GetVelocity();
    Velocity.Z = 0.f;

    if (Velocity.SizeSquared() < 1.f)
    {
        return EAllyAnimType::Idle;
    }

    FVector MoveDir = Velocity.GetSafeNormal();

    float ForwardDot = FVector::DotProduct(GetActorForwardVector(), MoveDir);
    float RightDot = FVector::DotProduct(GetActorRightVector(), MoveDir);

    if (ForwardDot > 0.1f)  return EAllyAnimType::Move_Front;
    if (ForwardDot < -0.1f) return EAllyAnimType::Move_Back;
    if (RightDot > 0.1f)    return EAllyAnimType::Move_Right;
    if (RightDot < -0.1f)   return EAllyAnimType::Move_Left;

    return EAllyAnimType::Move_Front;
}


void AAllyCharacter::FindNearestEnemy()
{
    // 既にターゲットがいるなら探さない
    if (TargetEnemy && IsValid(TargetEnemy)) return;

    float ClosestDistance = TNumericLimits<float>::Max();
    AEnemyCharacterBase* ClosestEnemy = nullptr;

    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
    {
        AEnemyCharacterBase* Enemy = *It;
        if (!Enemy) continue;

        float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestEnemy = Enemy;
        }
    }

    TargetEnemy = ClosestEnemy;
}

void AAllyCharacter::FaceTarget(AActor* Target)
{
    if (!Target) return;

    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0.f;

    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.f));
}

void AAllyCharacter::MoveBackToInitialPosition()
{
    FVector ToInitial = InitialPosition - GetActorLocation();
    ToInitial.Z = 0.f;

    if (ToInitial.Size() < AcceptableRadius)
    {
        StopMovement();
        return;
    }

    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        AICon->MoveToLocation(InitialPosition, AcceptableRadius);
       // PlayAnimation(EAllyAnimType::Move, true);
    }
    else
    {
        AddMovementInput(ToInitial.GetSafeNormal(), 1.f);
    }
}

//void AAllyCharacter::MoveAwayFromEnemy(AActor* Target)
//{
//    if (!Target) return;
//
//    FVector MoveDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
//    FVector Start = GetActorLocation();
//    FVector End = Start + MoveDir * 150.f;
//
//    // 壁回避
//    FHitResult Hit;
//    FCollisionQueryParams Params;
//    Params.AddIgnoredActor(this);
//
//    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
//    if (bHit)
//    {
//        FVector Right = FVector::CrossProduct(FVector::UpVector, MoveDir).GetSafeNormal();
//        FVector Left = -Right;
//
//        FVector CheckRightEnd = Start + Right * 100.f;
//        FVector CheckLeftEnd = Start + Left * 100.f;
//
//        bool bRightClear = !GetWorld()->LineTraceTestByChannel(Start, CheckRightEnd, ECC_WorldStatic, Params);
//        bool bLeftClear = !GetWorld()->LineTraceTestByChannel(Start, CheckLeftEnd, ECC_WorldStatic, Params);
//
//        if (bRightClear)
//            MoveDir = (MoveDir + Right * 0.5f).GetSafeNormal();
//        else if (bLeftClear)
//            MoveDir = (MoveDir + Left * 0.5f).GetSafeNormal();
//        else
//            MoveDir = FVector::ZeroVector;
//
//        //PlayAnimation(EAllyAnimType::Move, true);
//
//    }
//
//   /* if (!MoveDir.IsZero())
//        AddMovementInput(MoveDir, 1.f);*/
//
//    if (!Target) return;
//
//    FVector AwayDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
//    FVector Dest = GetActorLocation() + AwayDir * 600.f;
//
//    if (AAIController* AICon = Cast<AAIController>(GetController()))
//    {
//        AICon->MoveToLocation(Dest, AcceptableRadius);
//    }
//}

void AAllyCharacter::MoveAwayFromEnemy(AActor* Target)
{
    if (!Target) return;

    FVector AwayDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
    FVector Dest = GetActorLocation() + AwayDir * 800.f;

    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        AICon->MoveToLocation(Dest, AcceptableRadius);
    }
}


void AAllyCharacter::HandleFire()
{
    if (!EquippedWeapon) return;

    // ★ 移動停止
    StopMovement();
    //GetCharacterMovement()->StopMovementImmediately();
    //GetCharacterMovement()->DisableMovement(); // ← これが重要

    if (EquippedWeapon->GetAmmo() <= 0)
    {
        EquippedWeapon->StartReload();
        // ★ リロード中は逃走
        StartGetaway(GetawayTime);
        return;
    }
    if (EquippedWeapon->bIsFullAuto)
    {
        // フルオート開始
        EquippedWeapon->StartFire();
        PlayAnimation(EAllyAnimType::RangeAttack, false);
    }
    else
    {
        // セミオートは一発だけ
        EquippedWeapon->Fire();
        PlayAnimation(EAllyAnimType::RangeAttack, false);
    }
   
    GetWorldTimerManager().SetTimer(
        LockReleaseHandle,
        this,
        &AAllyCharacter::LockRelease,
        EquippedWeapon->FireRate,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("No Anime"));
    //EquippedWeapon->StartFire();
}

void AAllyCharacter::StopMovement()
{
    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        AICon->StopMovement();
        //PlayAnimation(EAllyAnimType::Idle, false);
    }
}

float AAllyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.f) return 0.f;

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
        *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    // ★ ダメージを受けたら逃走開始
    StartGetaway(GetawayTime);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }

    return ActualDamage;
}

void AAllyCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Ally %s died!"), *GetName());

    bIsDead = true;

    if (MHP != nullptr && MHP->AmmoWidget != nullptr)
    {
        MHP->AmmoWidget->UpdateDroneText(3);
    }

    if (DeadAnim && CurrentHealth <= 0)
    {
        /*  Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);*/
        PlayAnimation(EAllyAnimType::Dead, false);

        GetWorldTimerManager().SetTimer(
            LockReleaseHandle,
            this,
            &AAllyCharacter::LockRelease,
            1.8f,
            false
        );
        bIsDead = true;
    }
    else
    {
        

        Destroy();
    }

    
    DropCurrentWeapon();
    //Destroy();
}

void AAllyCharacter::DropCurrentWeapon()
{
    if (!EquippedWeapon) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 300.0f + FVector(0, 30, 50);
    FRotator DropRotation = FRotator(0, 0, 0);//GetActorRotation();

    // ▼ AItemBase をスポーン（変換ではない）
    AItemBase* DroppedItem = GetWorld()->SpawnActor<AItemBase>(
        AItemBase::StaticClass(),
        DropLocation,
        DropRotation,
        Params
    );

    if (DroppedItem)
    {
        DroppedItem->bIsWeapon = true;

        // ▼ 武器クラスをコピー
        DroppedItem->WeaponClass = EquippedWeapon->GetClass();

        // ▼ 弾数コピー
        DroppedItem->SavedAmmo = EquippedWeapon->Ammo;
        DroppedItem->SavedStockAmmo = EquippedWeapon->StockAmmo;
        DroppedItem->ItemType = EItemType::IT_Weapon;
        // ▼ メッシュの見た目を WeaponBase と合わせる（必要なら）
        if (EquippedWeapon->GetMesh())
        {
            DroppedItem->Mesh->SetStaticMesh(EquippedWeapon->GetMesh()->GetStaticMesh());
        }

        DroppedItem->Mesh->SetSimulatePhysics(true);
        DroppedItem->Mesh->AddImpulse(GetActorForwardVector() * 200);

        /*DroppedItem->Mesh->BodyInstance.bLockXRotation = true;
        DroppedItem->Mesh->BodyInstance.bLockYRotation = true;
        DroppedItem->Mesh->BodyInstance.bLockZRotation = true;*/

    }

    // ▼ 元の武器を破棄
    EquippedWeapon->Destroy();
    EquippedWeapon = nullptr;
}

UAnimationAsset* AAllyCharacter::GetAnimByType(EAllyAnimType Type) const
{
    switch (Type)
    {
    case EAllyAnimType::Idle:   return IdleAnim;
    case EAllyAnimType::Move_Front:  return MoveFrontAnim;
    case EAllyAnimType::Move_Back:   return MoveBackAnim;
    case EAllyAnimType::Move_Left:   return MoveLeftAnim;
    case EAllyAnimType::Move_Right:  return MoveRightAnim;
    case EAllyAnimType::Attack: return AttackAnim;
    case EAllyAnimType::RangeAttack: return RangeAttackAnim;
    case EAllyAnimType::Dead:   return DeadAnim;
    case EAllyAnimType::Damage:   return DamageAnim;
    default:                     return nullptr;


    }
}

void AAllyCharacter::PlayAnimation(EAllyAnimType NewType, bool bLoop)
{

    if (CurrentAnimType == NewType) return;

    // ロック中は無視
    if (bIsAnimationLocked)
    {
        return;
    }

    UAnimationAsset* Anim = GetAnimByType(NewType);
    if (!Anim) {
        UE_LOG(LogTemp, Warning, TEXT("No Anime"));
        return;
    }

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

bool AAllyCharacter::IsLockedAnim(EAllyAnimType Type) const
{
    return Type == EAllyAnimType::Attack
        || Type == EAllyAnimType::Dead
        || Type == EAllyAnimType::Damage;
        //|| Type == EAllyAnimType::RangeAttack;
}

void AAllyCharacter::LockRelease()
{

    bIsAnimationLocked = false;
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    if (bIsDead)
    {
       // PlayNiagaraEffect();

        Destroy();
    }
    else
    {
       
       // GetCharacterMovement()->MaxWalkSpeed = MoveEnemySpeed;
        

        UE_LOG(LogTemp, Warning, TEXT("Damage Anime end"));
    }

}

void AAllyCharacter::StartGetaway(float Duration)
{
    if (bIsDead || bIsGetaway) return;

    bIsGetaway = true;
    bIsGetawayMoving = false;

    GetWorldTimerManager().ClearTimer(FireTimerHandle);

    GetWorldTimerManager().SetTimer(
        GetawayTimerHandle,
        this,
        &AAllyCharacter::EndGetaway,
        Duration,
        false
    );
}

void AAllyCharacter::EndGetaway()
{
    bIsGetaway = false;
    bIsGetawayMoving = false;
}

//#include "AllyCharacter.h"
//#include "AIController.h"
//#include "TimerManager.h"
//#include "EngineUtils.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "DrawDebugHelpers.h"
//#include "NavigationSystem.h"     // UNavigationSystemV1
//#include "NavigationPath.h"       // FNavLocation
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//    AIControllerClass = AAllyAIController::StaticClass();
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    CurrentHealth = MaxHealth;
//    InitialPosition = GetActorLocation(); // 初期位置を保存
//
//    if (WeaponClass)
//    {
//        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
//        if (EquippedWeapon)
//        {
//            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
//            EquippedWeapon->SetOwner(this);
//        }
//    }
//
//    // 敵更新タイマー
//    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
//}
//
////void AAllyCharacter::Tick(float DeltaTime)
////{
////    Super::Tick(DeltaTime);
////
////    bool bShouldReturnHome = true;
////    float DistanceToEnemy = 0.f;
////
////    if (TargetEnemy && IsValid(TargetEnemy))
////    {
////        DistanceToEnemy = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());
////
////        if (IsEnemyVisible(TargetEnemy))
////            bShouldReturnHome = false; // 視認可能かつ到達可能なら戦闘
////    }
////
////    if (bShouldReturnHome)
////    {
////        // 初期位置へ戻る（壁回避付き）
////        FVector ToInitial = InitialPosition - GetActorLocation();
////        ToInitial.Z = 0.f;
////
////        if (ToInitial.Size() > AcceptableRadius)
////        {
////            FVector MoveDir = ToInitial.GetSafeNormal();
////
////            // 壁回避
////            FHitResult Hit;
////            FVector Start = GetActorLocation();
////            FVector End = Start + MoveDir * 100.f;
////            FCollisionQueryParams Params;
////            Params.AddIgnoredActor(this);
////
////            bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
////
////            if (bHit)
////            {
////                FVector Right = FVector::CrossProduct(FVector::UpVector, MoveDir).GetSafeNormal();
////                FVector Left = -Right;
////
////                FVector CheckRightEnd = Start + Right * 100.f;
////                FVector CheckLeftEnd = Start + Left * 100.f;
////
////                bool bRightClear = !GetWorld()->LineTraceTestByChannel(Start, CheckRightEnd, ECC_WorldStatic, Params);
////                bool bLeftClear = !GetWorld()->LineTraceTestByChannel(Start, CheckLeftEnd, ECC_WorldStatic, Params);
////
////                if (bRightClear)
////                    MoveDir = (MoveDir + Right * 0.5f).GetSafeNormal();
////                else if (bLeftClear)
////                    MoveDir = (MoveDir + Left * 0.5f).GetSafeNormal();
////                else
////                    MoveDir = FVector::ZeroVector;
////            }
////
////            if (!MoveDir.IsZero())
////                AddMovementInput(MoveDir, 1.f);
////        }
////
////        GetWorldTimerManager().ClearTimer(FireTimerHandle); // 戦闘停止
////        return;
////    }
////
////    // 敵が視認可能なら戦闘
////    FaceTarget(TargetEnemy);
////
////    if (DistanceToEnemy < MaintainDistance)
////    {
////        FVector ToTarget = (TargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
////        FVector MoveDirection = -ToTarget;
////
////        // 壁回避ロジック（既存のまま）
////        FHitResult Hit;
////        FVector Start = GetActorLocation();
////        FVector End = Start + MoveDirection * 100.f;
////        FCollisionQueryParams Params;
////        Params.AddIgnoredActor(this);
////
////        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
////
////        if (bHit)
////        {
////            FVector Right = FVector::CrossProduct(FVector::UpVector, MoveDirection).GetSafeNormal();
////            FVector Left = -Right;
////
////            FVector CheckRightEnd = Start + Right * 100.f;
////            FVector CheckLeftEnd = Start + Left * 100.f;
////
////            bool bRightClear = !GetWorld()->LineTraceTestByChannel(Start, CheckRightEnd, ECC_WorldStatic, Params);
////            bool bLeftClear = !GetWorld()->LineTraceTestByChannel(Start, CheckLeftEnd, ECC_WorldStatic, Params);
////
////            if (bRightClear)
////                MoveDirection = (MoveDirection + Right * 0.5f).GetSafeNormal();
////            else if (bLeftClear)
////                MoveDirection = (MoveDirection + Left * 0.5f).GetSafeNormal();
////            else
////                MoveDirection = FVector::ZeroVector;
////        }
////
////        if (!MoveDirection.IsZero())
////            AddMovementInput(MoveDirection, 1.0f);
////    }
////
////    // 射撃
////    if (DistanceToEnemy <= FireRange)
////    {
////        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
////            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
////    }
////    else
////    {
////        GetWorldTimerManager().ClearTimer(FireTimerHandle);
////    }
////}
//
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    // 敵がいない、または敵が遠すぎる場合は初期位置に戻る
//    bool bShouldReturnHome = true;
//    float DistanceToEnemy = 0.f;
//    
//
//    if (TargetEnemy && IsValid(TargetEnemy))
//    {
//        DistanceToEnemy = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());
//        FaceTarget(TargetEnemy);
//        if (DistanceToEnemy <= EnemyDetectRange)
//            bShouldReturnHome = false; // 敵が範囲内にいる場合は戦闘
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//
//    }
//
//    if (bShouldReturnHome)
//    {
//        FVector ToInitial = InitialPosition - GetActorLocation();
//        ToInitial.Z = 0.f;
//
//        if (ToInitial.Size() > AcceptableRadius)
//        {
//            FVector MoveDir = ToInitial.GetSafeNormal();
//
//            // 壁回避
//            FHitResult Hit;
//            FVector Start = GetActorLocation();
//            FVector End = Start + MoveDir * 100.f;
//            FCollisionQueryParams Params;
//            Params.AddIgnoredActor(this);
//
//            bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
//
//            if (bHit)
//            {
//                FVector Right = FVector::CrossProduct(FVector::UpVector, MoveDir).GetSafeNormal();
//                FVector Left = -Right;
//
//                FVector CheckRightEnd = Start + Right * 100.f;
//                FVector CheckLeftEnd = Start + Left * 100.f;
//
//                bool bRightClear = !GetWorld()->LineTraceTestByChannel(Start, CheckRightEnd, ECC_WorldStatic, Params);
//                bool bLeftClear = !GetWorld()->LineTraceTestByChannel(Start, CheckLeftEnd, ECC_WorldStatic, Params);
//
//                if (bRightClear)
//                    MoveDir = (MoveDir + Right * 0.5f).GetSafeNormal();
//                else if (bLeftClear)
//                    MoveDir = (MoveDir + Left * 0.5f).GetSafeNormal();
//                else
//                    MoveDir = FVector::ZeroVector;
//            }
//
//            if (!MoveDir.IsZero())
//                AddMovementInput(MoveDir, 1.f);
//        }
//
//        GetWorldTimerManager().ClearTimer(FireTimerHandle); // 戦闘停止
//        return;
//    }
//
//    // --- 敵が範囲内の場合は戦闘 ---
//    FaceTarget(TargetEnemy);
//
//    if (DistanceToEnemy < MaintainDistance)
//    {
//        FVector ToTarget = (TargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//        FVector MoveDirection = -ToTarget;
//
//        // 壁回避（既存ロジック）
//        FHitResult Hit;
//        FVector Start = GetActorLocation();
//        FVector End = Start + MoveDirection * 100.f;
//        FCollisionQueryParams Params;
//        Params.AddIgnoredActor(this);
//
//        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
//
//        if (bHit)
//        {
//            FVector Right = FVector::CrossProduct(FVector::UpVector, MoveDirection).GetSafeNormal();
//            FVector Left = -Right;
//
//            FVector CheckRightEnd = Start + Right * 100.f;
//            FVector CheckLeftEnd = Start + Left * 100.f;
//
//            bool bRightClear = !GetWorld()->LineTraceTestByChannel(Start, CheckRightEnd, ECC_WorldStatic, Params);
//            bool bLeftClear = !GetWorld()->LineTraceTestByChannel(Start, CheckLeftEnd, ECC_WorldStatic, Params);
//
//            if (bRightClear)
//                MoveDirection = (MoveDirection + Right * 0.5f).GetSafeNormal();
//            else if (bLeftClear)
//                MoveDirection = (MoveDirection + Left * 0.5f).GetSafeNormal();
//            else
//                MoveDirection = FVector::ZeroVector;
//        }
//
//        if (!MoveDirection.IsZero())
//            AddMovementInput(MoveDirection, 1.0f);
//    }
//
//    // 射撃
//    if (DistanceToEnemy <= FireRange)
//    {
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    }
//}
//
//
////void AAllyCharacter::Tick(float DeltaTime)
////{
////    Super::Tick(DeltaTime);
////
////    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
////    {
////        GetWorldTimerManager().ClearTimer(FireTimerHandle);
////        return;
////    }
////
////    // 敵方向を向く
////    FaceTarget(TargetEnemy);
////
////    float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());
////
////    if (Distance < MaintainDistance)
////    {
////        FVector ToTarget = (TargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
////        FVector MoveDirection = -ToTarget; // 後退
////
////        // 壁回避チェック
////        FHitResult Hit;
////        FVector Start = GetActorLocation();
////        FVector End = Start + MoveDirection * 100.f;
////        FCollisionQueryParams Params;
////        Params.AddIgnoredActor(this);
////
////        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);
////
////        if (bHit)
////        {
////            FVector Right = FVector::CrossProduct(FVector::UpVector, MoveDirection).GetSafeNormal();
////            FVector Left = -Right;
////
////            FVector CheckRightEnd = Start + Right * 100.f;
////            FVector CheckLeftEnd = Start + Left * 100.f;
////
////            bool bRightClear = !GetWorld()->LineTraceTestByChannel(Start, CheckRightEnd, ECC_WorldStatic, Params);
////            bool bLeftClear = !GetWorld()->LineTraceTestByChannel(Start, CheckLeftEnd, ECC_WorldStatic, Params);
////
////            if (bRightClear)
////                MoveDirection = (MoveDirection + Right * 0.5f).GetSafeNormal();
////            else if (bLeftClear)
////                MoveDirection = (MoveDirection + Left * 0.5f).GetSafeNormal();
////            else
////                MoveDirection = FVector::ZeroVector; // 完全に塞がれた場合
////        }
////
////        if (!MoveDirection.IsZero())
////            AddMovementInput(MoveDirection, 1.0f);
////    }
////
////    // 射撃処理
////    if (Distance <= FireRange)
////    {
////        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
////            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
////    }
////    else
////    {
////        GetWorldTimerManager().ClearTimer(FireTimerHandle);
////    }
////}
//
////bool AAllyCharacter::IsEnemyVisible(AEnemyCharacterBase* Enemy)
////{
////    if (!Enemy || !IsValid(Enemy))
////        return false;
////
////    FVector ToEnemy = Enemy->GetActorLocation() - GetActorLocation();
////    ToEnemy.Z = 0.f;
////
////    // 距離チェック
////    if (ToEnemy.Size() > SightDistance)
////        return false;
////
////    // 視界角度チェック
////    FVector Forward = GetActorForwardVector();
////    Forward.Z = 0.f;
////    float AngleDeg = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Forward.GetSafeNormal(), ToEnemy.GetSafeNormal())));
////    if (AngleDeg > SightAngle / 2.f)
////        return false;
////
////    // 壁越し判定（ライン・オブ・サイト）
////    FHitResult Hit;
////    FVector Start = GetActorLocation() + FVector(0, 0, 50.f);
////    FVector End = Enemy->GetActorLocation() + FVector(0, 0, 50.f);
////    FCollisionQueryParams Params;
////    Params.AddIgnoredActor(this);
////
////    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
////    if (bHit && Hit.GetActor() != Enemy)
////        return false;
////
////    // NavMesh 到達可能チェック（UE5 用）
////    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
////    if (!NavSys)
////        return false;
////
////    // Start から Enemy への経路を取得
////    UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), Enemy->GetActorLocation(), this);
////    if (!NavPath || NavPath->PathPoints.Num() == 0)
////        return false; // 経路なし＝到達不可
////
////    return true;
////}
//
//
//
//void AAllyCharacter::FaceTarget(AActor* Target)
//{
//    if (!Target) return;
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    ToTarget.Z = 0.f;
//    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
//    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.f));
//}
//
//void AAllyCharacter::FindNearestEnemy()
//{
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//        float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//}
//
//void AAllyCharacter::HandleFire()
//{
//    if (!EquippedWeapon) return;
//    if (EquippedWeapon->GetAmmo() <= 0)
//    {
//        EquippedWeapon->StartReload();
//        return;
//    }
//    EquippedWeapon->StartFire();
//}
//
//void AAllyCharacter::StopMovement()
//{
//    if (AAIController* AICon = Cast<AAIController>(GetController()))
//        AICon->StopMovement();
//}
//
//void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
//{
//    TargetEnemy = Cast<AEnemyCharacterBase>(NewTarget);
//}
//
//float AAllyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//    if (CurrentHealth <= 0.f) return 0.f;
//
//    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//    CurrentHealth -= ActualDamage;
//
//    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"), *GetName(), ActualDamage, CurrentHealth, MaxHealth);
//
//    if (CurrentHealth <= 0.f)
//        Die();
//
//    return ActualDamage;
//}
//
//void AAllyCharacter::Die()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Ally %s died!"), *GetName());
//    Destroy();
//}


//#include "AllyCharacter.h"
//#include "TimerManager.h"
//#include "EngineUtils.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "AAllyAIController.h"
//#include "NavigationSystem.h"     // UNavigationSystemV1
//#include "NavigationPath.h"       // FNavLocation
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//    AIControllerClass = AAllyAIController::StaticClass(); // ←専用AIControllerを使用
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    CurrentHealth = MaxHealth;
//
//    if (WeaponClass)
//    {
//        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
//        if (EquippedWeapon)
//        {
//            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
//            EquippedWeapon->SetOwner(this);
//        }
//    }
//
//    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
//}
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
//
//    // 敵方向を向く
//    FaceTarget(TargetEnemy);
//
//    float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());
//
//    // 近づかれたら後退
//    if (Distance < MaintainDistance)
//    {
//        FVector ToTarget = (TargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//        AddMovementInput(-ToTarget, 1.0f); // ←直接後退
//    }
//
//    // 射撃処理
//    if (Distance <= FireRange)
//    {
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//        {
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//        }
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    }
//}
//
////void AAllyCharacter::MoveToSafeDistance(AActor* Target, float DesiredDistance)
////{
////    if (!Target) return;
////
////    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
////    FVector Destination = GetActorLocation() - ToTarget * (DesiredDistance); // ←シンプルに維持距離分後退
////
////    if (AAllyAIController* AICon = Cast<AAllyAIController>(GetController()))
////    {
////        AICon->MoveToLocation(Destination, AcceptableRadius);
////    }
////}
//
//
//
//void AAllyCharacter::MoveToSafeDistance(AActor* Target, float DesiredDistance)
//{
//    if (!Target) return;
//
//    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//
//    // NavMesh 上で後退方向に移動可能な地点を探す
//    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
//    if (NavSys)
//    {
//        FNavLocation ResultLocation;
//        FVector ProposedLocation = GetActorLocation() - ToTarget * DesiredDistance;
//
//        // NavMesh 上の最も近い地点にスナップ
//        if (NavSys->ProjectPointToNavigation(ProposedLocation, ResultLocation, FVector(100.f, 100.f, 300.f)))
//        {
//            if (AAllyAIController* AICon = Cast<AAllyAIController>(GetController()))
//            {
//                AICon->MoveToLocation(ResultLocation.Location, AcceptableRadius);
//            }
//        }
//    }
//}
//
//
//void AAllyCharacter::FaceTarget(AActor* Target)
//{
//    if (!Target) return;
//
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    ToTarget.Z = 0.f;
//    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
//    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.f));
//}
//
//void AAllyCharacter::FindNearestEnemy()
//{
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//
//        float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//}
//
//void AAllyCharacter::HandleFire()
//{
//    if (!EquippedWeapon) return;
//
//    if (EquippedWeapon->GetAmmo() <= 0)
//    {
//        EquippedWeapon->StartReload();
//        return;
//    }
//
//    EquippedWeapon->StartFire();
//}
//
//void AAllyCharacter::StopMovement()
//{
//    if (AAllyAIController* AICon = Cast<AAllyAIController>(GetController()))
//    {
//        AICon->StopMovement();
//    }
//}
//
//void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
//{
//    TargetEnemy = Cast<AEnemyCharacterBase>(NewTarget);
//}
//
//float AAllyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
//    AController* EventInstigator, AActor* DamageCauser)
//{
//    if (CurrentHealth <= 0.f) return 0.f;
//
//    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//    CurrentHealth -= ActualDamage;
//
//    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
//        *GetName(), ActualDamage, CurrentHealth, MaxHealth);
//
//    if (CurrentHealth <= 0.f)
//    {
//        Die();
//    }
//
//    return ActualDamage;
//}
//
//void AAllyCharacter::Die()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Ally %s died!"), *GetName());
//    Destroy();
//}




//#include "AllyCharacter.h"
//#include "AIController.h"
//#include "WeaponBase.h"
//#include "EnemyCharacterBase.h"
//#include "TimerManager.h"
//#include "EngineUtils.h"
//#include "Kismet/KismetMathLibrary.h"
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//    AIControllerClass = AAIController::StaticClass();
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    CurrentHealth = MaxHealth;
//
//    // 武器生成
//    if (WeaponClass)
//    {
//        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
//        if (EquippedWeapon)
//        {
//            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
//            EquippedWeapon->SetOwner(this);
//        }
//    }
//
//    // 敵検出タイマー
//    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
//}
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
//
//    // 敵方向を常に向く
//    FaceTarget(TargetEnemy);
//
//    // 敵との距離をチェック
//    float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());
//
//    // 距離を維持して移動
//    if (FMath::Abs(Distance - MaintainDistance) > AcceptableRadius)
//    {
//        MoveToSafeDistance(TargetEnemy, MaintainDistance);
//    }
//    else
//    {
//        StopMovement();
//    }
//
//    // 射撃制御
//    if (Distance <= FireRange)
//    {
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//        {
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//        }
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    }
//}
//
//void AAllyCharacter::MoveToSafeDistance(AActor* Target, float DesiredDistance)
//{
//    if (!Target) return;
//
//    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//    FVector Destination = Target->GetActorLocation() - ToTarget * DesiredDistance;
//
//    // 目的地に向かって移動
//    if (AAIController* AICon = Cast<AAIController>(GetController()))
//    {
//        AICon->MoveToLocation(Destination, AcceptableRadius);
//    }
//}
//
//void AAllyCharacter::FaceTarget(AActor* Target)
//{
//    if (!Target) return;
//
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    ToTarget.Z = 0.f;
//    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
//    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.f));
//}
//
//void AAllyCharacter::FindNearestEnemy()
//{
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//
//        float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//}
//
//void AAllyCharacter::HandleFire()
//{
//    if (!EquippedWeapon) return;
//
//    if (EquippedWeapon->GetAmmo() <= 0)
//    {
//        EquippedWeapon->StartReload();
//        return;
//    }
//
//    EquippedWeapon->StartFire();
//}
//
//void AAllyCharacter::StopMovement()
//{
//    if (AAIController* AICon = Cast<AAIController>(GetController()))
//    {
//        AICon->StopMovement();
//    }
//}
//
//void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
//{
//    TargetEnemy = Cast<AEnemyCharacterBase>(NewTarget);
//}
//
//float AAllyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
//    AController* EventInstigator, AActor* DamageCauser)
//{
//    if (CurrentHealth <= 0.f) return 0.f;
//
//    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//    CurrentHealth -= ActualDamage;
//
//    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
//        *GetName(), ActualDamage, CurrentHealth, MaxHealth);
//
//    if (CurrentHealth <= 0.f)
//    {
//        Die();
//    }
//
//    return ActualDamage;
//}
//
//void AAllyCharacter::Die()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Ally %s died!"), *GetName());
//    Destroy();
//}





//#include "AllyCharacter.h"
//#include "AIController.h"
//#include "NavigationSystem.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "TimerManager.h"
//#include "EngineUtils.h"
//#include "EnemyCharacterBase.h"
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//    AIControllerClass = AAIController::StaticClass();
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    // 武器をスポーン＆装備
//    if (WeaponClass)
//    {
//        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
//        if (EquippedWeapon)
//        {
//            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
//            EquippedWeapon->SetOwner(this);
//        }
//    }
//
//    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
//
//    GetWorldTimerManager().SetTimer(MoveUpdateTimer, this, &AAllyCharacter::UpdateMovement, MoveUpdateInterval, true);
//
//
//    // ==== ランダムオフセット更新タイマーを開始 ====
//    GetWorldTimerManager().SetTimer(DirectionUpdateTimer, this, &AAllyCharacter::UpdateRandomOffset, DirectionUpdateInterval, true);
//    UpdateRandomOffset(); // 初期化
//
//    CurrentHealth = MaxHealth;
//}
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
//
//    FaceTarget(TargetEnemy);
//
//    float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());
//
//    // 射撃制御
//    if (Distance <= FireRange)
//    {
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//        {
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//        }
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    }
//
//    //FindNearestEnemy();
//
//    //if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
//    //{
//    //    GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    //    return;
//    //}
//
//    //FaceTarget(TargetEnemy);
//
//    //FVector EnemyLocation = TargetEnemy->GetActorLocation();
//    //FVector MyLocation = GetActorLocation();
//    //float Distance = FVector::Dist(EnemyLocation, MyLocation);
//
//    //if (Distance > MaintainDistance + AcceptableRadius)
//    //{
//    //    MoveTowards(TargetEnemy, MaintainDistance);
//    //}
//    //else if (Distance < MaintainDistance - AcceptableRadius)
//    //{
//    //    MoveAwayFrom(TargetEnemy, MaintainDistance);
//    //}
//    //else
//    //{
//    //    StopMovement();
//    //}
//
//    //// 射撃制御
//    //if (Distance <= FireRange)
//    //{
//    //    if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//    //    {
//    //        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//    //    }
//    //}
//    //else
//    //{
//    //    GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    //}
//}
//
//void AAllyCharacter::UpdateRandomOffset()
//{
//    // 毎回少し異なるランダムな方向を生成
//    float RandomYaw = FMath::FRandRange(-ZigzagAngleRange, ZigzagAngleRange);
//    FRotator RandRot(0.f, RandomYaw, 0.f);
//    CurrentOffsetDirection = RandRot.RotateVector(FVector::ForwardVector);
//
//    // さらに小さなランダム揺れを追加
//    FVector RandomJitter = FVector(
//        FMath::FRandRange(-1.0f, 1.0f),
//        FMath::FRandRange(-1.0f, 1.0f),
//        0.0f
//    ).GetSafeNormal();
//
//    CurrentOffsetDirection += RandomJitter * 0.3f;
//    CurrentOffsetDirection.Normalize();
//}
//
//void AAllyCharacter::MoveTowards(AActor* Target, float DesiredDistance)
//{
//    AAIController* AICon = Cast<AAIController>(GetController());
//    if (!AICon || !Target) return;
//
//    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//
//    // ターゲット方向にランダムオフセットを追加して「変則移動」
//    FVector MoveDir = (ToTarget + CurrentOffsetDirection * 0.4f).GetSafeNormal();
//
//    FVector Destination = Target->GetActorLocation() - MoveDir * DesiredDistance;
//
//    // 少し位置を揺らす
//    Destination += FVector(
//        FMath::FRandRange(-RandomOffsetRadius, RandomOffsetRadius),
//        FMath::FRandRange(-RandomOffsetRadius, RandomOffsetRadius),
//        0.0f
//    ) * 0.1f;
//
//    AICon->MoveToLocation(Destination, AcceptableRadius);
//}
//
//
//
//void AAllyCharacter::MoveAwayFrom(AActor* Target, float DesiredDistance)
//{
//    AAIController* AICon = Cast<AAIController>(GetController());
//    if (!AICon || !Target) return;
//
//    FVector MyLocation = GetActorLocation();
//    FVector TargetLocation = Target->GetActorLocation();
//    FVector ToTarget = (TargetLocation - MyLocation).GetSafeNormal();
//
//    // 横方向
//    FVector RightDir = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
//    float Side = FMath::FRandRange(0.f, 1.f) < 0.5f ? 1.f : -1.f;
//
//    // 基本的に敵方向を向きつつ少し後退
//    FVector BaseMove = -ToTarget * 1.0f + RightDir * 0.3f * Side;
//
//    // ランダムオフセット追加
//    FVector MoveDir = (BaseMove + CurrentOffsetDirection * 0.2f).GetSafeNormal();
//
//    // 最低移動距離を必ず確保（Destinationは現在位置から200~400ユニット離す）
//    float MoveDistance = FMath::FRandRange(200.f, 400.f);
//
//    FVector Destination = MyLocation + MoveDir * MoveDistance;
//
//    AICon->MoveToLocation(Destination, 5.f); // AcceptableRadiusは小さめに
//
//    // 敵方向を向く
//    FaceTarget(Target);
//
//   /* AAIController* AICon = Cast<AAIController>(GetController());
//    if (!AICon || !Target) return;
//
//    FVector AwayDir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
//    FVector MoveDir = (AwayDir + CurrentOffsetDirection * 0.3f).GetSafeNormal();
//
//    FVector Destination = GetActorLocation() + MoveDir * DesiredDistance;
//
//    Destination += FVector(
//        FMath::FRandRange(-RandomOffsetRadius, RandomOffsetRadius),
//        FMath::FRandRange(-RandomOffsetRadius, RandomOffsetRadius),
//        0.0f
//    ) * 0.1f;
//
//    AICon->MoveToLocation(Destination, AcceptableRadius);*/
//}
//
//void AAllyCharacter::FindNearestEnemy()
//{
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//
//        float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//}
//
//void AAllyCharacter::FaceTarget(AActor* Target)
//{
//    if (!Target) return;
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    ToTarget.Z = 0.0f;
//    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
//    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
//}
//
//void AAllyCharacter::HandleFire()
//{
//    if (!EquippedWeapon) return;
//
//    if (EquippedWeapon->GetAmmo() <= 0)
//    {
//        EquippedWeapon->StartReload();
//        return;
//    }
//
//    EquippedWeapon->StartFire();
//}
//void AAllyCharacter::StopMovement()
//{
//    AAIController* AICon = Cast<AAIController>(GetController());
//    if (AICon)
//    {
//        AICon->StopMovement();
//    }
//}
//
//void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
//{
//    TargetEnemy = NewTarget;
//}
//
//
//float AAllyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
//    AController* EventInstigator, AActor* DamageCauser)
//{
//    if (CurrentHealth <= 0.f) return 0.f;
//
//    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//    CurrentHealth -= ActualDamage;
//
//    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
//        *GetName(), ActualDamage, CurrentHealth, MaxHealth);
//
//    if (CurrentHealth <= 0.f)
//    {
//        Die();
//    }
//
//    return ActualDamage;
//}
//
//void AAllyCharacter::Die()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Ally %s died!"), *GetName());
//
//    // TODO: 死亡アニメーションやエフェクト再生
//    Destroy();
//}

//#include "AllyCharacter.h"
//#include "AIController.h"
//#include "NavigationSystem.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "TimerManager.h"
//#include "EngineUtils.h"
//#include "EnemyCharacterBase.h"
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//    AIControllerClass = AAIController::StaticClass();
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    // 武器をスポーン＆装備
//    if (WeaponClass)
//    {
//        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
//        if (EquippedWeapon)
//        {
//            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
//            EquippedWeapon->SetOwner(this);
//        }
//    }
//
//    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
//}
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    // 毎フレーム最も近い敵を探索
//    FindNearestEnemy();
//
//    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
//
//    // 敵の方向を向く
//    FaceTarget(TargetEnemy);
//
//    FVector EnemyLocation = TargetEnemy->GetActorLocation();
//    FVector MyLocation = GetActorLocation();
//    float Distance = FVector::Dist(EnemyLocation, MyLocation);
//
//    // 距離維持
//    if (Distance > MaintainDistance + AcceptableRadius)
//    {
//        MoveTowards(TargetEnemy, MaintainDistance);
//    }
//    else if (Distance < MaintainDistance - AcceptableRadius)
//    {
//        MoveAwayFrom(TargetEnemy, MaintainDistance);
//    }
//    else
//    {
//        StopMovement();
//    }
//
//    // 射撃制御
//    if (Distance <= FireRange)
//    {
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//        {
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//        }
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    }
//}
//
//void AAllyCharacter::FindNearestEnemy()
//{
//    float ClosestDistance = TNumericLimits<float>::Max();
//    AEnemyCharacterBase* ClosestEnemy = nullptr;
//
//    for (TActorIterator<AEnemyCharacterBase> It(GetWorld()); It; ++It)
//    {
//        AEnemyCharacterBase* Enemy = *It;
//        if (!Enemy) continue;
//
//        float Distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
//        if (Distance < ClosestDistance)
//        {
//            ClosestDistance = Distance;
//            ClosestEnemy = Enemy;
//        }
//    }
//
//    TargetEnemy = ClosestEnemy;
//}
//
//void AAllyCharacter::FaceTarget(AActor* Target)
//{
//    if (!Target) return;
//    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
//    ToTarget.Z = 0.0f;
//    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
//    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
//}
//
//void AAllyCharacter::HandleFire()
//{
//    if (!EquippedWeapon) return;
//
//    if (EquippedWeapon->GetAmmo() <= 0)
//    {
//        EquippedWeapon->StartReload();
//        return;
//    }
//
//    EquippedWeapon->StartFire();
//}
//
//void AAllyCharacter::MoveTowards(AActor* Target, float DesiredDistance)
//{
//    AAIController* AICon = Cast<AAIController>(GetController());
//    if (!AICon || !Target) return;
//
//    FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//    FVector Dest = Target->GetActorLocation() - Dir * DesiredDistance;
//    AICon->MoveToLocation(Dest, AcceptableRadius);
//}
//
//void AAllyCharacter::MoveAwayFrom(AActor* Target, float DesiredDistance)
//{
//    AAIController* AICon = Cast<AAIController>(GetController());
//    if (!AICon || !Target) return;
//
//    FVector Dir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
//    FVector Dest = GetActorLocation() + Dir * DesiredDistance;
//    AICon->MoveToLocation(Dest, AcceptableRadius);
//}
//
//void AAllyCharacter::StopMovement()
//{
//    AAIController* AICon = Cast<AAIController>(GetController());
//    if (AICon)
//    {
//        AICon->StopMovement();
//    }
//}
//
//void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
//{
//    TargetEnemy = NewTarget;
//}


//#include "AllyCharacter.h"
//#include "AIController.h"
//#include "NavigationSystem.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "TimerManager.h"
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    // 武器スポーン＆装備
//    if (WeaponClass)
//    {
//        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
//        if (EquippedWeapon)
//        {
//            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
//            EquippedWeapon->SetOwner(this);
//        }
//    }
//}
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (!TargetEnemy || !EquippedWeapon) return;
//
//    FVector EnemyLocation = TargetEnemy->GetActorLocation();
//    FVector MyLocation = GetActorLocation();
//    float Distance = FVector::Dist(EnemyLocation, MyLocation);
//
//    // 敵との距離に応じて移動
//    if (Distance > MaintainDistance + 50.f)
//    {
//        MoveTowards(TargetEnemy, MaintainDistance);
//    }
//    else if (Distance < MaintainDistance - 50.f)
//    {
//        MoveAwayFrom(TargetEnemy, MaintainDistance);
//    }
//    else
//    {
//        StopMovement();
//    }
//
//    // 射撃範囲内なら攻撃開始
//    if (Distance <= FireRange)
//    {
//        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
//        {
//            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
//        }
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//    }
//}
//
//void AAllyCharacter::HandleFire()
//{
//    if (!EquippedWeapon) return;
//
//    // 弾切れなら自動リロード
//    if (EquippedWeapon->GetAmmo() <= 0.0f)
//    {
//        EquippedWeapon->StartReload();
//        return;
//    }
//
//    EquippedWeapon->StartFire();
//}
//
//void AAllyCharacter::MoveTowards(AActor* Target, float DesiredDistance)
//{
//    if (!Controller || !Target) return;
//
//    FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//    FVector Destination = Target->GetActorLocation() - Direction * DesiredDistance;
//
//    UNavigationSystemV1* NavSys = FNavigationSystemV1::GetCurrent<UNavigationSystemV1>(GetWorld());
//    if (NavSys)
//    {
//        NavSys->SimpleMoveToLocation(Controller, Destination);
//    }
//}
//
//void AAllyCharacter::MoveAwayFrom(AActor* Target, float DesiredDistance)
//{
//    if (!Controller || !Target) return;
//
//    FVector Direction = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
//    FVector Destination = Target->GetActorLocation() + Direction * DesiredDistance;
//
//    UNavigationSystemV1* NavSys = FNavigationSystemV1::GetCurrent<UNavigationSystemV1>(GetWorld());
//    if (NavSys)
//    {
//        NavSys->SimpleMoveToLocation(Controller, Destination);
//    }
//}
//
//void AAllyCharacter::StopMovement()
//{
//    if (Controller)
//    {
//        Controller->StopMovement();
//    }
//}
//
//void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
//{
//    TargetEnemy = NewTarget;
//}
//
//
//#include "AllyCharacter.h"
//#include "Kismet/GameplayStatics.h"
//#include "TimerManager.h"
//#include "Engine/World.h"
//#include "EnemyCharacterBase.h" // 敵キャラのクラス
//
//AAllyCharacter::AAllyCharacter()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AAllyCharacter::BeginPlay()
//{
//    Super::BeginPlay();
//
//    // 武器をスポーンして装備
//    if (WeaponClass)
//    {
//        FActorSpawnParameters SpawnParams;
//        SpawnParams.Owner = this;
//        SpawnParams.Instigator = GetInstigator();
//
//        AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(
//            WeaponClass,
//            FVector::ZeroVector,
//            FRotator::ZeroRotator,
//            SpawnParams
//        );
//
//        if (SpawnedWeapon)
//        {
//            EquipWeapon(SpawnedWeapon);
//        }
//    }
//
//    // 攻撃ループを開始
//    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::FireAtTarget, FireInterval, true);
//}
//
//void AAllyCharacter::EquipWeapon(AWeaponBase* NewWeapon)
//{
//    if (!NewWeapon) return;
//
//    EquippedWeapon = NewWeapon;
//
//    EquippedWeapon->AttachToComponent(
//        GetMesh(),
//        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
//        WeaponAttachSocketName
//    );
//
//    EquippedWeapon->SetOwner(this);
//}
//
//void AAllyCharacter::FireAtTarget()
//{
//    if (!EquippedWeapon) return;
//
//    AActor* Target = FindNearestEnemy();
//    if (!Target) return;
//
//    // ターゲットの方向に向ける
//    FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//    FRotator LookRotation = Direction.Rotation();
//    SetActorRotation(FRotator(0.0f, LookRotation.Yaw, 0.0f)); // Yaw だけ向ける
//
//    EquippedWeapon->StartFire();
//}
//
//AActor* AAllyCharacter::FindNearestEnemy()
//{
//    AActor* NearestEnemy = nullptr;
//    float NearestDistance = TNumericLimits<float>::Max();
//
//    TArray<AActor*> AllEnemies;
//    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacterBase::StaticClass(), AllEnemies);
//
//    for (AActor* Enemy : AllEnemies)
//    {
//        float Dist = FVector::Dist(Enemy->GetActorLocation(), GetActorLocation());
//        if (Dist < NearestDistance)
//        {
//            NearestDistance = Dist;
//            NearestEnemy = Enemy;
//        }
//    }
//
//    return NearestEnemy;
//}
//
//void AAllyCharacter::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (EquippedWeapon)
//    {
//        if (EquippedWeapon->GetAmmo() <= 0 && EquippedWeapon->GetStockAmmo() > 0)
//        {
//            EquippedWeapon->StartReload();
//        }
//    }
//}
