#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include "DefenseBase.h"
#include "MyHeroPlayer.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController BeginPlay"));

    // ñhâqëŒè€ÇíTÇ∑
    TArray<AActor*> FoundBases;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADefenseBase::StaticClass(), FoundBases);

    if (FoundBases.Num() > 0)
    {
        TargetActor = FoundBases[0]; // ç≈èâÇÃñhâqëŒè€Çñ⁄ìIínÇ…
        MoveToActor(TargetActor, 50.0f);
        UE_LOG(LogTemp, Warning, TEXT("EnemyAI target set to DefenseBase: %s"), *TargetActor->GetName());
    }

}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    UE_LOG(LogTemp, Warning, TEXT("EnemyAIController possessed: %s"), *InPawn->GetName());

    if (PlayerPawn && InPawn)
    {
        MoveToActor(PlayerPawn, 0.0f); // ìûíBîºåa100
    }
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    AEnemyCharacterBase* Enemy = Cast<AEnemyCharacterBase>(GetPawn());
    if (!Enemy) return;

    AActor* Target = Enemy->ChooseTarget_Default();
    if (Target)
    {
        MoveToActor(Target, 50.f);
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
//    MoveToActor(PlayerPawn, 100.0f); // 100cmà»ì‡Ç≈í‚é~
//
//    //  MoveToActor ÇÃåãâ ÇïœêîÇ≈éÛÇØéÊÇÈ
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
//        MoveToActor(PlayerPawn, 3.f); // 5.fÇÕìûíBîºåa
//    }
//}
