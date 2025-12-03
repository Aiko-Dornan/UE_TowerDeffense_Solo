#include "DroneSpawner.h"
#include "DroneCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

ADroneSpawner::ADroneSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ADroneSpawner::BeginPlay()
{
    Super::BeginPlay();

   
        // タイマーで定期スポーン
        GetWorld()->GetTimerManager().SetTimer(
            SpawnTimer,
            this,
            &ADroneSpawner::SpawnDrone,
            SpawnInterval,
            true
        );
       
    
   
    

    
}

void ADroneSpawner::SpawnDrone()
{
    if (!DroneClass) return;

    // ランダムなXY位置（円形）
    do
    {
        RandomOffset =
            UKismetMathLibrary::RandomPointInBoundingBox(
                FVector::ZeroVector,
                FVector(SpawnRadius, SpawnRadius, 0)
            );
    } while (SpawnRadius<5000.0f);

   

    // Z は spawner の高さをそのまま使用
    FVector SpawnPos = GetActorLocation() + RandomOffset;
    FRotator SpawnRot = FRotator::ZeroRotator;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ADroneCharacter* Drone = GetWorld()->SpawnActor<ADroneCharacter>(
        DroneClass,
        SpawnPos,
        SpawnRot,
        Params
    );

    if (Drone)
    {
        Drone->SpawnDefaultController();
        UE_LOG(LogTemp, Warning, TEXT("Drone spawned at: %s"), *SpawnPos.ToString());
    }

    NowSpawn++;

    if (NowSpawn > SpawnLimit)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
    }
}
