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

    // ���퐶��
    if (WeaponClass)
    {
        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
        if (EquippedWeapon)
        {
            EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
            EquippedWeapon->SetOwner(this);
        }
    }

    // �G���o�^�C�}�[
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

    // �G��������Ɍ���
    FaceTarget(TargetEnemy);

    // �G�Ƃ̋������`�F�b�N
    float Distance = FVector::Dist(GetActorLocation(), TargetEnemy->GetActorLocation());

    // �������ێ����Ĉړ�
    if (FMath::Abs(Distance - MaintainDistance) > AcceptableRadius)
    {
        MoveToSafeDistance(TargetEnemy, MaintainDistance);
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

void AAllyCharacter::MoveToSafeDistance(AActor* Target, float DesiredDistance)
{
    if (!Target) return;

    FVector ToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FVector Destination = Target->GetActorLocation() - ToTarget * DesiredDistance;

    // �ړI�n�Ɍ������Ĉړ�
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
//    // ������X�|�[��������
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
//    // ==== �����_���I�t�Z�b�g�X�V�^�C�}�[���J�n ====
//    GetWorldTimerManager().SetTimer(DirectionUpdateTimer, this, &AAllyCharacter::UpdateRandomOffset, DirectionUpdateInterval, true);
//    UpdateRandomOffset(); // ������
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
//    // �ˌ�����
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
//    //// �ˌ�����
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
//    // ���񏭂��قȂ郉���_���ȕ����𐶐�
//    float RandomYaw = FMath::FRandRange(-ZigzagAngleRange, ZigzagAngleRange);
//    FRotator RandRot(0.f, RandomYaw, 0.f);
//    CurrentOffsetDirection = RandRot.RotateVector(FVector::ForwardVector);
//
//    // ����ɏ����ȃ����_���h���ǉ�
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
//    // �^�[�Q�b�g�����Ƀ����_���I�t�Z�b�g��ǉ����āu�ϑ��ړ��v
//    FVector MoveDir = (ToTarget + CurrentOffsetDirection * 0.4f).GetSafeNormal();
//
//    FVector Destination = Target->GetActorLocation() - MoveDir * DesiredDistance;
//
//    // �����ʒu��h�炷
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
//    // ������
//    FVector RightDir = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
//    float Side = FMath::FRandRange(0.f, 1.f) < 0.5f ? 1.f : -1.f;
//
//    // ��{�I�ɓG�����������������
//    FVector BaseMove = -ToTarget * 1.0f + RightDir * 0.3f * Side;
//
//    // �����_���I�t�Z�b�g�ǉ�
//    FVector MoveDir = (BaseMove + CurrentOffsetDirection * 0.2f).GetSafeNormal();
//
//    // �Œ�ړ�������K���m�ہiDestination�͌��݈ʒu����200~400���j�b�g�����j
//    float MoveDistance = FMath::FRandRange(200.f, 400.f);
//
//    FVector Destination = MyLocation + MoveDir * MoveDistance;
//
//    AICon->MoveToLocation(Destination, 5.f); // AcceptableRadius�͏����߂�
//
//    // �G����������
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
//    // TODO: ���S�A�j���[�V������G�t�F�N�g�Đ�
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
//    // ������X�|�[��������
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
//    // ���t���[���ł��߂��G��T��
//    FindNearestEnemy();
//
//    if (!TargetEnemy || !IsValid(TargetEnemy) || !EquippedWeapon)
//    {
//        GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
//
//    // �G�̕���������
//    FaceTarget(TargetEnemy);
//
//    FVector EnemyLocation = TargetEnemy->GetActorLocation();
//    FVector MyLocation = GetActorLocation();
//    float Distance = FVector::Dist(EnemyLocation, MyLocation);
//
//    // �����ێ�
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
//    // �ˌ�����
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
