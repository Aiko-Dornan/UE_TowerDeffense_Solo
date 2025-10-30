#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "MyHeroPlayer.h"
#include"AllyCharacter.h"
#include"DefenseBase.h"
#include"TD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "UObject/WeakObjectPtr.h"

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackRange = 150.0f;
    AttackCooldown = 2.0f;
    AttackDamage = 10.0f;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;

    GetCharacterMovement()->MaxWalkSpeed = 600.f;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


}

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<AMyHeroPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    CurrentHealth = MaxHealth;
}

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead || !PlayerCharacter) return;

    // 1. �U���Ώۂ�I��
    AActor* Target = ChooseTarget();

    if (!Target) return;

    CurrentTarget = Target;

    // 2. �����`�F�b�N
    const float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    //float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance <= AttackRange)
    {
        PerformAttack();
    }

    

}

AActor* AEnemyCharacterBase::ChooseTarget()
{
    // 1. �����L�����N�^�[�����ׂĎ擾
    TArray<AActor*> AllAllies;

    // �v���C���[
    if (PlayerCharacter)
    {
        AllAllies.Add(PlayerCharacter);
    }

    // ���̖����L�����N�^�[
    TArray<AActor*> FoundAllies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);
    AllAllies.Append(FoundAllies);

    // 2. �ł��߂�������T��
    AActor* NearestAlly = nullptr;
    float NearestDistance = FLT_MAX;

    for (AActor* Ally : AllAllies)
    {
        if (!IsValid(Ally)) continue;

        float Dist = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());
        if (Dist < NearestDistance)
        {
            NearestDistance = Dist;
            NearestAlly = Ally;
        }
    }

    if (NearestAlly)
    {
        return NearestAlly; // �ł��߂��������^�[�Q�b�g��
    }

    // 3. ���������Ȃ���Ζh�q�Ώہi��n�j��T��
    TArray<AActor*> FoundBases;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);

    if (FoundBases.Num() > 0)
    {
        return FoundBases[0];
    }



   // TArray<AActor*> FoundAllies;
   // UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);

   // if (FoundAllies.Num() > 0)
   // {
   //     return FoundAllies[0]; // �ŏ��̖�����I��
   // }

   // // �D��x1�F�v���C���[���߂��Ȃ�v���C���[��_��
   // if (IsValid(PlayerCharacter))
   // {
   //     const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
   //     if (DistanceToPlayer <= 800.f) // ��: 8m�ȓ��Ȃ�D��
   //     {
   //         return PlayerCharacter;
   //     }
   // }

   //  // 2. �����L�����N�^�[���������ċ����ŒZ�̂��̂�I��
   //// TArray<AActor*> FoundAllies;
   // UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAllyCharacter::StaticClass(), FoundAllies);

   // AActor* ClosestAlly = nullptr;
   // float ClosestDistance = FLT_MAX;

   // for (AActor* Ally : FoundAllies)
   // {
   //     // �������g�͏��O
   //     //if (Ally == this) continue;

   //     // �����`�F�b�N
   //     AAllyCharacter* AllyChar = Cast<AAllyCharacter>(Ally);
   //     if (!AllyChar || IsValid(AllyChar)) continue;

   //     const float DistanceToAlly = FVector::Dist(GetActorLocation(), Ally->GetActorLocation());

   //     // 800cm�ȓ��̖��������Ώ�
   //     if (DistanceToAlly <= 2000.f && DistanceToAlly < ClosestDistance)
   //     {
   //         ClosestDistance = DistanceToAlly;
   //         ClosestAlly = Ally;
   //     }
   // }

   // if (ClosestAlly)
   // {
   //     return ClosestAlly;
   // }

   // // �D��x2�F�h�q�Ώہi��n�j��T��
   // TArray<AActor*> FoundBases;
   // UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);

   // if (FoundBases.Num() > 0)
   // {
   //     return FoundBases[0]; // �ŏ��̊�n���^�[�Q�b�g��
   // }

    return nullptr;
}

void AEnemyCharacterBase::PerformAttack()
{

    if (!bCanAttack || bIsDead || !CurrentTarget) return;

    bCanAttack = false;

    UE_LOG(LogTemp, Warning, TEXT("%s attacks %s!"), *GetName(), *CurrentTarget->GetName());

    UGameplayStatics::ApplyDamage(CurrentTarget, AttackDamage, GetController(), this, nullptr);

    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);


    //if (!bCanAttack || !PlayerCharacter || bIsDead) return;

    //AActor* Target = PlayerCharacter;
    //if (!Target)
    //{
    //    // �h�q�Ώۂ������i�v���C���[�����Ȃ��ꍇ�j
    //    TArray<AActor*> FoundBases;
    //    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);
    //    if (FoundBases.Num() > 0)
    //    {
    //        Target = FoundBases[0];
    //    }
    //}

    //if (!Target) return;

    //bCanAttack = false;

    //UE_LOG(LogTemp, Warning, TEXT("Enemy attacks target: %s"), *Target->GetName());

    //UGameplayStatics::ApplyDamage(PlayerCharacter, AttackDamage, GetController(), this, nullptr);

    //GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
}

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

    return ActualDamage;
}

void AEnemyCharacterBase::Die()
{
    if (bIsDead) return;
    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy %s died!"), *GetName());

    // TODO: �A�j���[�V������G�t�F�N�g��ǉ��������ꍇ�͂����ōĐ�
    // PlayAnimMontage(DeathAnimMontage);

    // ��莞�Ԍ�ɏ��ł��Ă��悢
    //SetLifeSpan(2.0f);

    // GameMode�ɒʒm
    ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
    {
        GM->OnEnemyDestroyed();
    }

    // �����ɏ����ꍇ��
     Destroy();
}
