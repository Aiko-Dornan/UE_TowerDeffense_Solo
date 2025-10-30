#include "AllyCharacter.h"
#include "AIController.h"
#include "WeaponBase.h"
#include "EnemyCharacterBase.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"

AAllyCharacter::AAllyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AAllyCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // 武器生成
    if (WeaponClass)
    {
        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
        if (EquippedWeapon)
        {
            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
            EquippedWeapon->SetOwner(this);
        }
    }

    // 敵検出タイマー
    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
}

void AAllyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        return;
    }

    // 敵方向を常に向く
    FaceTarget(TargetEnemy);

    // 敵との距離をチェック
    float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

    // 距離を維持して移動
    if (FMath::Abs(Distance - MaintainDistance) > AcceptableRadius)
    {
        MoveToSafeDistance(TargetEnemy, MaintainDistance);
    }
    else
    {
        StopMovement();
    }

    // 射撃制御
    if (Distance <= FireRange)
    {
        if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
        {
            GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAllyCharacter::HandleFire, FireInterval, true);
        }
    }
    else
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
    }
}

void AAllyCharacter::MoveToSafeDistance(AActor* Target, float DesiredDistance)
{
    if (!Target) return;

    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FVector Destination = Target->GetActorLocation() - ToTarget * DesiredDistance;

    // 目的地に向かって移動
    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        AICon->MoveToLocation(Destination, AcceptableRadius);
    }
}

void AAllyCharacter::FaceTarget(AActor* Target)
{
    if (!Target) return;

    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0.f;
    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.f));
}

void AAllyCharacter::FindNearestEnemy()
{
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

void AAllyCharacter::HandleFire()
{
    if (!EquippedWeapon) return;

    if (EquippedWeapon->GetAmmo() <= 0)
    {
        EquippedWeapon->StartReload();
        return;
    }

    EquippedWeapon->StartFire();
}

void AAllyCharacter::StopMovement()
{
    if (AAIController* AICon = Cast<AAIController>(GetController()))
    {
        AICon->StopMovement();
    }
}

void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
{
    TargetEnemy = Cast<AEnemyCharacterBase>(NewTarget);
}

float AAllyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.f) return 0.f;

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
        *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }

    return ActualDamage;
}

void AAllyCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Ally %s died!"), *GetName());
    Destroy();
}





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
