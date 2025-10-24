#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController BeginPlay"));
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController possessed: %s"), *InPawn->GetName());

    if (PlayerPawn && InPawn)
    {
        MoveToActor(PlayerPawn, 0.0f); // ���B���a100
    }
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (PlayerPawn)
    {
        MoveToActor(PlayerPawn, 0.0f);
    }
}


//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//
//AEnemyAIController::AEnemyAIController()
//{
//    PrimaryActorTick.bCanEverTick = true;
//}
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController BeginPlay"));
//}
//
//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    //UE_LOG(LogTemp, Warning, TEXT("EnemyAIController Tick"));
//
//    if (PlayerPawn)
//    {
//        MoveToActor(PlayerPawn, 100.0f);
//    }
//}
//
//void AEnemyAIController::OnPossess(APawn* InPawn)
//{
//    Super::OnPossess(InPawn);
//
//    UE_LOG(LogTemp, Warning, TEXT("AIController possessed pawn: %s"), *InPawn->GetName());
//
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//
//    if (PlayerPawn)
//    {
//        MoveToActor(PlayerPawn, 100.0f);
//    }
//}


//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//#include "NavigationSystem.h"
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//}
//
//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    if (!PlayerPawn) return;
//
//    MoveToActor(PlayerPawn, 100.0f); // 100cm�ȓ��Œ�~
//
//    //  MoveToActor �̌��ʂ�ϐ��Ŏ󂯎��
//    EPathFollowingRequestResult::Type MoveResult = MoveToActor(PlayerPawn, 100.0f);
//
//    switch (MoveResult)
//    {
//    case EPathFollowingRequestResult::Failed:
//        UE_LOG(LogTemp, Warning, TEXT("AI MoveToActor FAILED: %s"), *GetName());
//        break;
//
//    case EPathFollowingRequestResult::RequestSuccessful:
//        UE_LOG(LogTemp, Warning, TEXT("AI moving towards player..."));
//        break;
//
//    default:
//        UE_LOG(LogTemp, Warning, TEXT("AI unknown MoveResult."));
//        break;
//    }
//}


//#include "EnemyAIController.h"
//#include "Kismet/GameplayStatics.h"
//#include "GameFramework/Character.h"
//#include "NavigationSystem.h"
//
//void AEnemyAIController::BeginPlay()
//{
//    Super::BeginPlay();
//
//    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
//}
//
//void AEnemyAIController::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//    if (PlayerPawn)
//    {
//        MoveToActor(PlayerPawn, 3.f); // 5.f�͓��B���a
//    }
//}
