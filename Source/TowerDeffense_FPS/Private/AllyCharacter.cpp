#include "AllyCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "EnemyCharacterBase.h"

AAllyCharacter::AAllyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();
}

void AAllyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // ������X�|�[��������
    if (WeaponClass)
    {
        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
        if (EquippedWeapon)
        {
            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
            EquippedWeapon->SetOwner(this);
        }
    }

    GetWorldTimerManager().SetTimer(TargetUpdateTimer, this, &AAllyCharacter::FindNearestEnemy, 0.5f, true);
}

void AAllyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���t���[���ł��߂��G��T��
    FindNearestEnemy();

    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        return;
    }

    // �G�̕���������
    FaceTarget(TargetEnemy);

    FVector EnemyLocation = TargetEnemy->GetActorLocation();
    FVector MyLocation = GetActorLocation();
    float Distance = FVector::Dist(EnemyLocation, MyLocation);

    // �����ێ�
    if (Distance > MaintainDistance + AcceptableRadius)
    {
        MoveTowards(TargetEnemy, MaintainDistance);
    }
    else if (Distance < MaintainDistance - AcceptableRadius)
    {
        MoveAwayFrom(TargetEnemy, MaintainDistance);
    }
    else
    {
        StopMovement();
    }

    // �ˌ�����
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

void AAllyCharacter::FaceTarget(AActor* Target)
{
    if (!Target) return;
    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    ToTarget.Z = 0.0f;
    FRotator LookRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
    SetActorRotation(FMath::RInterpTo(GetActorRotation(), LookRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
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

void AAllyCharacter::MoveTowards(AActor* Target, float DesiredDistance)
{
    AAIController* AICon = Cast<AAIController>(GetController());
    if (!AICon || !Target) return;

    FVector Dir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FVector Dest = Target->GetActorLocation() - Dir * DesiredDistance;
    AICon->MoveToLocation(Dest, AcceptableRadius);
}

void AAllyCharacter::MoveAwayFrom(AActor* Target, float DesiredDistance)
{
    AAIController* AICon = Cast<AAIController>(GetController());
    if (!AICon || !Target) return;

    FVector Dir = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
    FVector Dest = GetActorLocation() + Dir * DesiredDistance;
    AICon->MoveToLocation(Dest, AcceptableRadius);
}

void AAllyCharacter::StopMovement()
{
    AAIController* AICon = Cast<AAIController>(GetController());
    if (AICon)
    {
        AICon->StopMovement();
    }
}

void AAllyCharacter::SetTargetEnemy(AActor* NewTarget)
{
    TargetEnemy = NewTarget;
}


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
//    // ����X�|�[��������
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
//    // �G�Ƃ̋����ɉ����Ĉړ�
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
//    // �ˌ��͈͓��Ȃ�U���J�n
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
//    // �e�؂�Ȃ玩�������[�h
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
//#include "EnemyCharacterBase.h" // �G�L�����̃N���X
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
//    // ������X�|�[�����đ���
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
//    // �U�����[�v���J�n
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
//    // �^�[�Q�b�g�̕����Ɍ�����
//    FVector Direction = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//    FRotator LookRotation = Direction.Rotation();
//    SetActorRotation(FRotator(0.0f, LookRotation.Yaw, 0.0f)); // Yaw ����������
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
