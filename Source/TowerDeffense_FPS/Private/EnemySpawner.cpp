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

    // ���R���|�[�l���g�i�G�f�B�^�Ō������m�F���₷���j
    SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
    SpawnPoint->SetupAttachment(RootComponent);
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();

    // ���Ԋu�œG���X�|�[��
    if (EnemyClass)
    {
        GetWorldTimerManager().SetTimer(
            SpawnTimerHandle,
            this,
            &AEnemySpawner::SpawnEnemy,
            SpawnInterval,
            true,      // �J��Ԃ�
            0.0f       // �ŏ��̃X�|�[���x��
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
