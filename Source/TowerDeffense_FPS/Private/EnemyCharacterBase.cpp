#include "EnemyCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include"EnemyAIController.h"

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackRange = 150.0f;    // �ߐڍU�������W
    AttackCooldown = 2.0f;   // �U���N�[���_�E���b��
    MoveSpeed = 5.0f;       //�ړ����x
    bCanAttack = true;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    GetCharacterMovement()->MaxWalkSpeed = 600.f;  // �D���ȑ��x�ɐݒ�
}

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);


}

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!PlayerPawn) return;

    FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
    float DistanceToPlayer = Direction.Size();

    if (DistanceToPlayer > AttackRange)
    {
        // �v���C���[�Ɍ������Ĉړ�
        MoveToPlayer();
    }
    else
    {
        // �U���͈͓��Ȃ�U��
        if (bCanAttack)
        {
            PerformAttack();
        }
        // ��~
        GetCharacterMovement()->StopMovementImmediately();
    }
}

void AEnemyCharacterBase::MoveToPlayer()
{
    if (!PlayerPawn) return;

    FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

    AddMovementInput(Direction, 1.0f);

    UE_LOG(LogTemp, Warning, TEXT("Movement!!"));
    UE_LOG(LogTemp, Warning, TEXT("%s"),*PlayerPawn->GetName());
}

void AEnemyCharacterBase::PerformAttack()
{
    bCanAttack = false;

    // �U��������F���O�o��
    UE_LOG(LogTemp, Warning, TEXT("Enemy attacks player!"));

    // �U���N�[���_�E���^�C�}�[���Z�b�g
    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
}
