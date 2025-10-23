#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (PlayerPawn)
    {
        MoveToActor(PlayerPawn, 3.f); // 5.f‚Í“ž’B”¼Œa
    }
}
