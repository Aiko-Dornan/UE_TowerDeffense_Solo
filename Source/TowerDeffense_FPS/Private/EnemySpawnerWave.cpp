#include "EnemySpawnerWave.h"
#include"TD_GameModeBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include"AmmoDisplayWidget.h"
#include"MyHeroPlayer.h"


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

    TArray<AActor*> FoundPlayer;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyHeroPlayer::StaticClass(), FoundPlayer);
    MHP = Cast<AMyHeroPlayer>(FoundPlayer[0]);

    GenerateRandomSpawnPoints();
    // 最初のウェーブを開始
    SpawnWave();
}

void AEnemySpawnerWave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //if (bWaveInProgress && AreAllEnemiesDead())
    //{
    //    bWaveInProgress = false;

    //   

    //    if (CurrentWave >= LimitWave)
    //    {
    //        

    //        if (ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    //        {
    //            GM->GameClear();
    //        }

    //        return;
    //    }

    //    // 残りウェーブがある場合
    //   
    //    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
    //}

    

}

void AEnemySpawnerWave::SpawnWave()
{
    all_spawn = false;

    //  最終ウェーブか？
    const bool bIsFinalWave = (CurrentWave >= LimitWave);

   

   

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

    //  このステージはボスありか？
    bool bSpawnBoss = false;
    if (UTD_GameInstance* GI = GetGameInstance<UTD_GameInstance>())
    {
        if (GI->BossSummon.IsValidIndex(GI->MaxClearedStage))
        {
            bSpawnBoss = GI->BossSummon[GI->MaxClearedStage];
        }

        // =========================
   //  ボス召喚
   // =========================
        if (bIsFinalWave && bSpawnBoss && BossEnemyClass[GI->MaxClearedStage])
        {
            // ランダムなスポーンポイントを選択
            FVector BossSpawnLocation = SpawnPoints[FMath::RandRange(1, SpawnPoints.Num() - 1)];

            // 少し位置をずらす
            BossSpawnLocation += FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);

            FRotator BossRotation = FRotator::ZeroRotator;

            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AEnemyCharacterBase* Boss =
                GetWorld()->SpawnActor<AEnemyCharacterBase>(
                    BossEnemyClass[GI->MaxClearedStage],
                    BossSpawnLocation,
                    BossRotation,
                    Params
                );



            if (Boss)
            {

                if (Boss->bIsBoss)
                {
                    // ボス用BGM
                    // HPバーUI表示
                    // カメラ演出
                }

                Boss->SpawnDefaultController();
                Boss->UpdateTarget();
                Boss->StartMovingToTarget();

                //  重要：通常敵と同じ管理に入れる
                SpawnedEnemies.Add(Boss);

                UE_LOG(LogTemp, Warning, TEXT("Boss Spawned!"));
            }
        }
    }

    //int32 EnemyCountThisWave = InitialEnemyCount + (CurrentWave - 1) * EnemyIncreasePerWave;
   
    int32 EnemyCountThisWave = SpawnPoints.Num() /** InitialEnemyCount*/;

    UE_LOG(LogTemp, Warning, TEXT("Wave %d started! Spawning %d enemies."), CurrentWave, EnemyCountThisWave);

    if (UTD_GameInstance* GI = GetGameInstance<UTD_GameInstance>())
    {
        for (int32 i = 0; i < EnemyCountThisWave; i++)
        {
            // 各敵のスポーンを少しずつずらす（0.2秒ごと）
            const float Delay = i * 0.2f;

            // ランダムなスポーンポイントを選択
            FVector SpawnLocation = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

            // 少し位置をずらす
            SpawnLocation += FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0);

            FRotator SpawnRotation = FRotator::ZeroRotator;

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            RandomInt = FMath::RandRange(0, 7);

            switch (RandomInt)
            {
            case 0:
                EnemyClass = EnemyClassTypeB[GI->MaxClearedStage];
                break;
            case 1:
                EnemyClass = EnemyClassTypeG[GI->MaxClearedStage];
                break;
            case 2:
                EnemyClass = EnemyClassTypeT[GI->MaxClearedStage];
                break;
            case 3:
                EnemyClass = EnemyClassTypeK[GI->MaxClearedStage];
                break;

            default:
                EnemyClass = EnemyClassTypeB[GI->MaxClearedStage];
                break;
            }

            AEnemyCharacterBase* SpawnedEnemy = World->SpawnActor<AEnemyCharacterBase>(
                EnemyClass,
                SpawnLocation,
                SpawnRotation,
                SpawnParams
            );

            if (SpawnedEnemy)
            {


                // まずAIを生成（この時点ではPossessされていない可能性あり）
                SpawnedEnemy->SpawnDefaultController();

                // 少し遅らせてターゲット更新と移動を開始
                FTimerHandle TempHandle;
                World->GetTimerManager().SetTimer(
                    TempHandle,
                    [SpawnedEnemy]()
                    {
                        if (!IsValid(SpawnedEnemy)) return;

                        // 念のためBaseStructureがnullでないか確認
                        SpawnedEnemy->UpdateTarget();
                        SpawnedEnemy->StartMovingToTarget();

                        UE_LOG(LogTemp, Warning, TEXT("%s AI started moving after delayed init."), *SpawnedEnemy->GetName());
                    },
                    0.25f,   // 0.25秒後に実行（AI Possess完了を待つ）
                    false
                );

                 // AI を有効化

                SpawnedEnemies.Add(SpawnedEnemy);
            }
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

   /* TArray<AActor*> FoundPlayer;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyHeroPlayer::StaticClass(), FoundPlayer);
    MHP = Cast<AMyHeroPlayer>(FoundPlayer[0]);

    if (MHP != nullptr && MHP->AmmoWidget != nullptr)
    {
        MHP->AmmoWidget->UpdateDroneText(3);
    }*/
    if (MHP != nullptr && MHP->AmmoWidget != nullptr)
    {
        MHP->AmmoWidget->UpdateDroneText(4);
    }
   // all_spawn = true;
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
        if (bWaveInProgress && AreAllEnemiesDead())
        {
            bWaveInProgress = false;
            UE_LOG(LogTemp, Warning, TEXT("All enemies dead on wave %d!"), CurrentWave);

            if (CurrentWave >= LimitWave)
            {
                GetWorldTimerManager().SetTimer(GameClearHandle, this, &AEnemySpawnerWave::GameClearAct, 5.0f, false);
              /*  if (ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
                {
                    GM->GameClear();
                }*/
            }
            else
            {
                // 次のウェーブを開始
                GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
            }
        }
    }
}

void AEnemySpawnerWave::GameClearAct()
{
    if (ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GM->GameClear();
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