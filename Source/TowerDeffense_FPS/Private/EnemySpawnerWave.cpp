#include "EnemySpawnerWave.h"
#include"TD_GameModeBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawnerWave::AEnemySpawnerWave()
{
    PrimaryActorTick.bCanEverTick = true;

    SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
    RootComponent = SpawnPoint;
}

void AEnemySpawnerWave::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("AEnemySpawnerWave::BeginPlay() called for: %s | IsPendingKill: %d | IsTemplate: %d"),
        *GetName(),
        IsPendingKillPending() ? 1 : 0,
        HasAnyFlags(RF_ClassDefaultObject) ? 1 : 0);


    GenerateRandomSpawnPoints();
    // 最初のウェーブを開始
    SpawnWave();
}

void AEnemySpawnerWave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bWaveInProgress && AreAllEnemiesDead())
    {
        bWaveInProgress = false;

        UE_LOG(LogTemp, Warning, TEXT("Wave %d cleared!"), CurrentWave);

        if (CurrentWave >= LimitWave)
        {
            UE_LOG(LogTemp, Warning, TEXT("Final wave cleared! Triggering GameClear()..."));

            if (ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
            {
                GM->GameClear();
            }

            return;
        }

        // 残りウェーブがある場合
        UE_LOG(LogTemp, Warning, TEXT("Next wave in %.1f seconds."), TimeAfterClear);
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
    }

    //if (bWaveInProgress && AreAllEnemiesDead())
    //{
    //    bWaveInProgress = false;

    //    // 最終ウェーブならゲームクリア
    //    if (CurrentWave >= LimitWave)
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("Final wave cleared! Game clear!"));

    //        ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    //        if (GM)
    //        {
    //            GM->GameClear(); // GameClear() を呼び出す
    //        }

    //        return; // もう次のウェーブに進まない
    //    }

    //    // まだ残りウェーブがある場合は次のウェーブへ
    //    UE_LOG(LogTemp, Warning, TEXT("All enemies defeated! Next wave in %.1f seconds."), TimeAfterClear);
    //    GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    //    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
    //}

}
void AEnemySpawnerWave::SpawnWave()
{
    // 上限チェック
    if (CurrentWave > LimitWave)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnWave() called but CurrentWave (%d) exceeds LimitWave (%d)."),
            CurrentWave, LimitWave);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("SpawnWave() called! CurrentWave: %d"), CurrentWave);

    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyClass is null!"));
        return;
    }

    if (SpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPoints is empty!"));
        //SpawnPoints.Add(GetActorLocation() + FVector(200, 0, 50));
        return;
    }

    if (!EnemyClass || SpawnPoints.Num() == 0) return;



    UWorld* World = GetWorld();
    if (!World) return;

    SpawnedEnemies.Empty();

   

    //int32 EnemyCountThisWave = InitialEnemyCount + (CurrentWave - 1) * EnemyIncreasePerWave;
   
    int32 EnemyCountThisWave = SpawnPoints.Num() /** InitialEnemyCount*/;

    UE_LOG(LogTemp, Warning, TEXT("Wave %d started! Spawning %d enemies."), CurrentWave, EnemyCountThisWave);

    for (int32 i = 0; i < EnemyCountThisWave; i++)
    {
        // ランダムなスポーンポイントを選択
        FVector SpawnLocation = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

        // 少し位置をずらす
        SpawnLocation += FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);

        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AEnemyCharacterBase* SpawnedEnemy = World->SpawnActor<AEnemyCharacterBase>(
            EnemyClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );

        if (SpawnedEnemy)
        {

        

           /*  SpawnedEnemy->SpawnDefaultController();

            if (SpawnedEnemy->GetController())
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy now has controller: %s"), *SpawnedEnemy->GetController()->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Enemy has NO controller!"));
                SpawnedEnemy->SpawnDefaultController();
            }*/


            // AI を有効化
           
            SpawnedEnemies.Add(SpawnedEnemy);
        }
    }

    bWaveInProgress = true;

    // ウェーブタイマーセット（時間経過で進行）
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeBetweenWaves, false);

    UE_LOG(LogTemp, Warning, TEXT("Wave %d started! SpawnPoints.Num = %d"), CurrentWave, SpawnPoints.Num());


    UE_LOG(LogTemp, Warning, TEXT("SpawnWave() called! CurrentWave: %d"), CurrentWave);
    // ウィジェットへ通知
    OnWaveChanged.Broadcast(CurrentWave); // 1引数の場合

  
}


void AEnemySpawnerWave::StartNextWave()
{
    // すでに最終ウェーブなら何もしない
    if (CurrentWave >= LimitWave)
    {
        UE_LOG(LogTemp, Warning, TEXT("All waves completed! No more enemies will spawn."));
        return;
    }

    
    //GenerateRandomSpawnPoints();
    CurrentWave++;

    //新ウェーブを通知
    //OnWaveChanged.Broadcast(CurrentWave, LimitWave);

    SpawnWave();
}

void AEnemySpawnerWave::NotifyEnemyDestroyed(AEnemyCharacterBase* DeadEnemy)
{
    SpawnedEnemies.Remove(DeadEnemy);

    UE_LOG(LogTemp, Warning, TEXT("Enemy removed: %s | Remaining: %d"),
        *DeadEnemy->GetName(), SpawnedEnemies.Num());

    // 全滅チェック
    if (SpawnedEnemies.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("All enemies dead on wave %d!"), CurrentWave);

        if (CurrentWave >= LimitWave)
        {
            if (ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
            {
                GM->GameClear();
            }
        }
        else
        {
            // 次のウェーブを開始
            GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
        }
    }
}

bool AEnemySpawnerWave::AreAllEnemiesDead() const
{
    for (AEnemyCharacterBase* Enemy : SpawnedEnemies)
    {
        if (IsValid(Enemy))
        {
            return false;
        }
    }
    return true;
}


//bool AEnemySpawnerWave::AreAllEnemiesDead() /*const*/
//{
//   
//
//    /* int32 AliveCount = 0;
//
//    for (int32 i = 0; i < SpawnedEnemies.Num(); i++)
//    {
//        AEnemyCharacterBase* Enemy = SpawnedEnemies[i];
//        if (IsValid(Enemy))
//        {
//            AliveCount++;
//            UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Enemy %d still alive: %s"), i, *Enemy->GetName());
//        }
//    }
//
//    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] AreAllEnemiesDead -> AliveCount: %d"), AliveCount);
//    return AliveCount == 0;*/
//
//    for (AEnemyCharacterBase* Enemy : SpawnedEnemies)
//    {
//        if (IsValid(Enemy)) // nullptr か PendingKill でなければ生存
//        {
//            return false;
//        }
//    }
//    UE_LOG(LogTemp, Warning, TEXT("All Enemy Dead!!!!!!!!!!!!!!!!!!!1."));
//    return true;
//
//   /* for (AEnemyCharacterBase* Enemy : SpawnedEnemies)
//    {
//        if (IsValid(Enemy))
//        {
//            return false;
//        }
//    }
//    return true;*/
//
//}

void AEnemySpawnerWave::GenerateRandomSpawnPoints()
{
    SpawnPoints.Empty();

    ADefenseBase* Base = Cast<ADefenseBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ADefenseBase::StaticClass()));
    if (!Base) return;

    FVector BaseLocation = Base->GetActorLocation();
    int32 SpawnedCount = 0;
    int32 MaxAttempts = 1000;
    int32 Attempts = 0;

    while (SpawnedCount < NumberOfSpawnPoints && Attempts < MaxAttempts)
    {
        Attempts++;

        // 360度ランダム方向
        float AngleDeg = FMath::FRandRange(0.f, 360.f);
        float AngleRad = FMath::DegreesToRadians(AngleDeg);

        float Distance = FMath::FRandRange(MinDistanceFromBase, MaxDistanceFromBase);

        FVector Offset(FMath::Cos(AngleRad) * Distance, FMath::Sin(AngleRad) * Distance, 0.f);
       

        // 湧き位置
        FVector SpawnLocation = BaseLocation + Offset;

        // Zを地面に合わせるなど調整（必要なら LineTrace）
        SpawnLocation.Z = BaseLocation.Z;

        SpawnPoints.Add(SpawnLocation);
        SpawnedCount++;
    }

    UE_LOG(LogTemp, Warning, TEXT("SpawnPoints generated: %d / %d"), SpawnedCount, NumberOfSpawnPoints);
}


//void AEnemySpawnerWave::GenerateRandomSpawnPoints()
//{
//    SpawnPoints.Empty();
//
//   
//
//    ADefenseBase* Base = Cast<ADefenseBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ADefenseBase::StaticClass()));
//    if (!Base) return;
//
//    int32 SpawnedCount = 0;
//    int32 MaxAttempts = 100;
//    int32 Attempts = 0;
//
//    while (SpawnedCount < NumberOfSpawnPoints)
//    {
//        float RandX = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
//        float RandY = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
//        float RandZ = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);
//        FVector CandidateLocation(RandX, RandY, RandZ);
//
//        FVector BaseLocation = Base->GetActorLocation();
//        float DistToBase = FVector::Dist(BaseLocation, CandidateLocation);
//        if (DistToBase >= MinDistanceFromBase || Attempts >= MaxAttempts)
//        {
//            SpawnPoints.Add(CandidateLocation);
//            SpawnedCount++;
//        }
//        
//        Attempts++;
//    }
//
//   
//    if (SpawnedCount < NumberOfSpawnPoints)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("SpawnPoints generated: %d / %d"), SpawnedCount, NumberOfSpawnPoints);
//    }
// 
//}