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

    GenerateRandomSpawnPoints();
    // 最初のウェーブを開始
    SpawnWave();
}

void AEnemySpawnerWave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 現在ウェーブ中で、敵が全滅していたら次ウェーブ進行タイマーをセット
    /*if (bWaveInProgress && AreAllEnemiesDead() && CurrentWave < LimitWave)
    {
        bWaveInProgress = false;
        UE_LOG(LogTemp, Warning, TEXT("All enemies defeated! Next wave in %.1f seconds."), TimeAfterClear);

        GetWorldTimerManager().ClearTimer(WaveTimerHandle);
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
    }*/

    if (bWaveInProgress && AreAllEnemiesDead())
    {
        bWaveInProgress = false;

        // 最終ウェーブならゲームクリア
        if (CurrentWave >= LimitWave)
        {
            UE_LOG(LogTemp, Warning, TEXT("Final wave cleared! Game clear!"));

            ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
            if (GM)
            {
                GM->GameClear(); // GameClear() を呼び出す
            }

            return; // もう次のウェーブに進まない
        }

        // まだ残りウェーブがある場合は次のウェーブへ
        UE_LOG(LogTemp, Warning, TEXT("All enemies defeated! Next wave in %.1f seconds."), TimeAfterClear);
        GetWorldTimerManager().ClearTimer(WaveTimerHandle);
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
    }

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

   

    int32 EnemyCountThisWave = InitialEnemyCount + (CurrentWave - 1) * EnemyIncreasePerWave;

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
            // AI を有効化
            SpawnedEnemy->SpawnDefaultController();
            SpawnedEnemies.Add(SpawnedEnemy);
        }
    }

    bWaveInProgress = true;

    // ウェーブタイマーセット（時間経過で進行）
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeBetweenWaves, false);

    UE_LOG(LogTemp, Warning, TEXT("Wave %d started! SpawnPoints.Num = %d"), CurrentWave, SpawnPoints.Num());

    //if (!EnemyClass) return;

    //UWorld* World = GetWorld();
    //if (!World) return;

    //// 古い敵リストをクリア
    //SpawnedEnemies.Empty();

    //int32 EnemyCountThisWave = InitialEnemyCount + (CurrentWave - 1) * EnemyIncreasePerWave;

    //UE_LOG(LogTemp, Warning, TEXT("Wave %d started! Spawning %d enemies."), CurrentWave, EnemyCountThisWave);

    //for (int32 i = 0; i < EnemyCountThisWave; i++)
    //{
    //    FVector SpawnLocation = SpawnPoint->GetComponentLocation();
    //    SpawnLocation += FVector(FMath::RandRange(-150, 150), FMath::RandRange(-150, 150), 0);
    //    FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

    //    FActorSpawnParameters SpawnParams;
    //    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //    AEnemyCharacterBase* SpawnedEnemy = World->SpawnActor<AEnemyCharacterBase>(
    //        EnemyClass,
    //        SpawnLocation,
    //        SpawnRotation,
    //        SpawnParams
    //    );

    //    if (SpawnedEnemy)
    //    {
    //        SpawnedEnemy->SpawnDefaultController();
    //        SpawnedEnemies.Add(SpawnedEnemy);
    //    }
    //}

    //bWaveInProgress = true;

    //// 通常ウェーブタイマーもセット（時間で進む用）
    //GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    //GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeBetweenWaves, false);


    //if (!EnemyClass) return;

    //UWorld* World = GetWorld();
    //if (!World) return;

    //int32 EnemyCountThisWave = InitialEnemyCount + (CurrentWave - 1) * EnemyIncreasePerWave;

    //UE_LOG(LogTemp, Warning, TEXT("Wave %d started! Spawning %d enemies."), CurrentWave, EnemyCountThisWave);

    //for (int32 i = 0; i < EnemyCountThisWave; i++)
    //{
    //    FVector SpawnLocation = SpawnPoint->GetComponentLocation();

    //    // 敵を少しずつずらしてスポーン（重ならないように）
    //    SpawnLocation += FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);

    //    FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

    //    FActorSpawnParameters SpawnParams;
    //    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //    AEnemyCharacterBase* SpawnedEnemy = World->SpawnActor<AEnemyCharacterBase>(
    //        EnemyClass,
    //        SpawnLocation,
    //        SpawnRotation,
    //        SpawnParams
    //    );

    //    if (SpawnedEnemy)
    //    {
    //        // AI を有効化（重要！）
    //        SpawnedEnemy->SpawnDefaultController();
    //    }
    //}

    //// 次のウェーブをセット
    //GetWorldTimerManager().SetTimer(
    //    WaveTimerHandle,
    //    this,
    //    &AEnemySpawnerWave::StartNextWave,
    //    TimeBetweenWaves,
    //    false
    //);
}


void AEnemySpawnerWave::StartNextWave()
{
    // すでに最終ウェーブなら何もしない
    if (CurrentWave >= LimitWave)
    {
        UE_LOG(LogTemp, Warning, TEXT("All waves completed! No more enemies will spawn."));
        return;
    }

    CurrentWave++;
    SpawnWave();
}

bool AEnemySpawnerWave::AreAllEnemiesDead() const
{
    for (AEnemyCharacterBase* Enemy : SpawnedEnemies)
    {
        if (IsValid(Enemy)) // nullptr か PendingKill でなければ生存
        {
            return false;
        }
    }
    return true;
}

void AEnemySpawnerWave::GenerateRandomSpawnPoints()
{
    SpawnPoints.Empty();

    /*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;*/

    ADefenseBase* Base = Cast<ADefenseBase>(UGameplayStatics::GetActorOfClass(GetWorld(), ADefenseBase::StaticClass()));
    if (!Base) return;

    int32 SpawnedCount = 0;
    int32 MaxAttempts = 100;
    int32 Attempts = 0;

    while (SpawnedCount < NumberOfSpawnPoints)
    {
        float RandX = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
        float RandY = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
        float RandZ = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);
        FVector CandidateLocation(RandX, RandY, RandZ);

        FVector BaseLocation = Base->GetActorLocation();
        float DistToBase = FVector::Dist(BaseLocation, CandidateLocation);
        if (DistToBase >= MinDistanceFromBase || Attempts >= MaxAttempts)
        {
            SpawnPoints.Add(CandidateLocation);
            SpawnedCount++;
        }
        /*float DistToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), CandidateLocation);
        if (DistToPlayer >= MinDistanceFromPlayer || Attempts >= MaxAttempts)
        {
            SpawnPoints.Add(CandidateLocation);
            SpawnedCount++;
        }*/
        Attempts++;
    }

    //while (SpawnedCount < NumberOfSpawnPoints && Attempts < MaxAttempts)
    //{
    //    Attempts++;

    //    float RandX = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
    //    float RandY = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
    //    float RandZ = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);
    //    FVector CandidateLocation(RandX, RandY, RandZ);

    //    if (FVector::Dist(PlayerPawn->GetActorLocation(), CandidateLocation) >= MinDistanceFromPlayer)
    //    {
    //        SpawnPoints.Add(CandidateLocation);
    //        SpawnedCount++;
    //        UE_LOG(LogTemp, Warning, TEXT("SpawnPoint %d: %s"), SpawnedCount, *CandidateLocation.ToString());
    //    }

    //    //// プレイヤーとの距離チェック
    //    //if (FVector::Dist(PlayerPawn->GetActorLocation(), CandidateLocation) >= MinDistanceFromPlayer)
    //    //{
    //    //    SpawnPoints.Add(CandidateLocation);
    //    //    SpawnedCount++;
    //    //}
    //}
    if (SpawnedCount < NumberOfSpawnPoints)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPoints generated: %d / %d"), SpawnedCount, NumberOfSpawnPoints);
    }
    /*if (SpawnedCount < NumberOfSpawnPoints)
    {
        UE_LOG(LogTemp, Warning, TEXT("十分な距離を保てるスポーンポイントを生成できませんでした"));
    }*/

    //SpawnPoints.Empty();

    //APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    //if (!PlayerPawn) return;

    //int32 SpawnedCount = 0;
    //int32 MaxAttempts = 100; // 無限ループ防止
    //int32 Attempts = 0;

    //while (SpawnedCount < NumberOfSpawnPoints && Attempts < MaxAttempts)
    //{
    //    Attempts++;

    //    // ランダム位置生成
    //    float RandX = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
    //    float RandY = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
    //    float RandZ = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);
    //    FVector CandidateLocation(RandX, RandY, RandZ);

    //    // プレイヤーとの距離をチェック
    //    if (FVector::Dist(PlayerPawn->GetActorLocation(), CandidateLocation) >= MinDistanceFromPlayer)
    //    {
    //        SpawnPoints.Add(CandidateLocation);
    //        SpawnedCount++;
    //    }
    //}

    //if (SpawnedCount < NumberOfSpawnPoints)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("十分な距離を保てるスポーンポイントを生成できませんでした"));
    //}
}