#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AEnemyCharacterBase::AEnemyCharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AttackRange = 150.0f;
    AttackCooldown = 2.0f;

    GetCharacterMovement()->MaxWalkSpeed = 600.f;

    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyCharacterBase::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyCharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter Tick"));
}

void AEnemyCharacterBase::PerformAttack()
{
    if (!bCanAttack) return;

    bCanAttack = false;
    UE_LOG(LogTemp, Warning, TEXT("Enemy attacks!"));

    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
}

void AEnemyCharacterBase::ResetAttack()
{
    bCanAttack = true;
}



//#include "EnemyCharacterBase.h"
//#include "Kismet/GameplayStatics.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "TimerManager.h"
//
//AEnemyCharacterBase::AEnemyCharacterBase()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AttackRange = 150.0f;
//    AttackCooldown = 2.0f;
//    MoveSpeed = 600.0f; // �L�����N�^�[�ړ����x
//    bCanAttack = true;
//
//    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
//}
//
//void AEnemyCharacterBase::BeginPlay()
//{
//    Super::BeginPlay();
//}
//
//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    
//
//    //UE_LOG(LogTemp, Warning, TEXT("EnemyCharacterBase Tick"));
//}
//
//void AEnemyCharacterBase::PerformAttack()
//{
//    if (!bCanAttack) return;
//
//    bCanAttack = false;
//
//    // �U��������
//    UE_LOG(LogTemp, Warning, TEXT("%s attacks player!"), *GetName());
//
//    // �U���N�[���_�E��
//    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
//}
//
//void AEnemyCharacterBase::ResetAttack()
//{
//    bCanAttack = true;
//}


//#include "EnemyCharacterBase.h"
//#include "Kismet/GameplayStatics.h"
//#include "EnemyAIController.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "TimerManager.h"
//
//AEnemyCharacterBase::AEnemyCharacterBase()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AIControllerClass = AEnemyAIController::StaticClass();
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//
//    
//
//    GetCharacterMovement()->bUseControllerDesiredRotation = true;
//    GetCharacterMovement()->bOrientRotationToMovement = true;
//    GetCharacterMovement()->MaxWalkSpeed = 300.f;
//}
//
//void AEnemyCharacterBase::BeginPlay()
//{
//    Super::BeginPlay();
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//}
//
//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (!PlayerPawn) return;
//
//    FVector MyLocation = GetActorLocation();
//    FVector PlayerLocation = PlayerPawn->GetActorLocation();
//    float Distance = FVector::Dist(MyLocation, PlayerLocation);
//
//    if (Distance <= AttackRange)
//    {
//        // �U���͈͂ɓ��������~�{�U��
//        AAIController* AICon = Cast<AAIController>(GetController());
//        if (AICon)
//        {
//            AICon->StopMovement();
//        }
//
//        if (bCanAttack)
//        {
//            PerformAttack();
//        }
//    }
//}
//
//void AEnemyCharacterBase::PerformAttack()
//{
//    bCanAttack = false;
//    UE_LOG(LogTemp, Warning, TEXT("%s attacks player!"), *GetName());
//
//    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
//}
//
//void AEnemyCharacterBase::ResetAttack()
//{
//    bCanAttack = true;
//}


//#include "EnemyCharacterBase.h"
//#include "Kismet/GameplayStatics.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "TimerManager.h"
//#include"EnemyAIController.h"
//
//AEnemyCharacterBase::AEnemyCharacterBase()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    AttackRange = 150.0f;    // �ߐڍU�������W
//    AttackCooldown = 2.0f;   // �U���N�[���_�E���b��
//    MoveSpeed = 5.0f;       //�ړ����x
//    bCanAttack = true;
//
//    AIControllerClass = AEnemyAIController::StaticClass();
//    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
//
//    GetCharacterMovement()->MaxWalkSpeed = 600.f;  // �D���ȑ��x�ɐݒ�
//}
//
//void AEnemyCharacterBase::BeginPlay()
//{
//    Super::BeginPlay();
//
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//
//
//}
//
//void AEnemyCharacterBase::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    if (!PlayerPawn) return;
//
//    FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
//    float DistanceToPlayer = Direction.Size();
//
//    if (DistanceToPlayer > AttackRange)
//    {
//        // �v���C���[�Ɍ������Ĉړ�
//        MoveToPlayer();
//    }
//    else
//    {
//        // �U���͈͓��Ȃ�U��
//        if (bCanAttack)
//        {
//            PerformAttack();
//        }
//        // ��~
//        GetCharacterMovement()->StopMovementImmediately();
//    }
//}
//
//void AEnemyCharacterBase::MoveToPlayer()
//{
//    if (!PlayerPawn) return;
//
//    FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
//
//    AddMovementInput(Direction, 1.0f);
//
//   // UE_LOG(LogTemp, Warning, TEXT("Movement!!"));
//   // UE_LOG(LogTemp, Warning, TEXT("%s"),*PlayerPawn->GetName());
//}
//
//void AEnemyCharacterBase::PerformAttack()
//{
//    bCanAttack = false;
//
//    // �U��������F���O�o��
//    UE_LOG(LogTemp, Warning, TEXT("Enemy attacks player!"));
//
//    // �U���N�[���_�E���^�C�}�[���Z�b�g
//    GetWorldTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AEnemyCharacterBase::ResetAttack, AttackCooldown, false);
//}
//
//void AEnemyCharacterBase::ResetAttack()
//{
//    bCanAttack = true;
//}
