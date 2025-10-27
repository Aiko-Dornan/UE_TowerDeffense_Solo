#include "EnemySpawner.h"
#include "EnemyCharacterBase.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // 矢印コンポーネント（エディタで向きを確認しやすい）
    SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
    SpawnPoint->SetupAttachment(RootComponent);
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    // 一定間隔で敵をスポーン
    if (EnemyClass)
    {
        GetWorldTimerManager().SetTimer(
            SpawnTimerHandle,
            this,
            &AEnemySpawner::SpawnEnemy,
            SpawnInterval,
            true,      // 繰り返し
            0.0f       // 最初のスポーン遅延
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyClass is not set in %s"), *GetName());
    }

    
}

void AEnemySpawner::SpawnEnemy()
{
    if (!EnemyClass) return;
    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = nullptr;

    FVector SpawnLocation = SpawnPoint->GetComponentLocation();
    FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

    AEnemyCharacterBase* SpawnedEnemy = World->SpawnActor<AEnemyCharacterBase>(
        EnemyClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    if (SpawnedEnemy)
    {
        SpawnedEnemy->SpawnDefaultController();
        UE_LOG(LogTemp, Log, TEXT("Enemy spawned and AI should possess it."));
    }
}
