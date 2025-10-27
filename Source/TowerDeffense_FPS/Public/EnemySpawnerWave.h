#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCharacterBase.h"
#include "EnemySpawnerWave.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemySpawnerWave : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawnerWave();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 敵を実際にスポーンする関数
    void SpawnWave();

    void GenerateRandomSpawnPoints();

    // 次のウェーブへ進行
    void StartNextWave();

    // 敵が全滅しているか確認
    bool AreAllEnemiesDead() const;

public:
    // 敵のクラス
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClass;

    // スポーン位置
    UPROPERTY(EditAnywhere, Category = "Spawn")
    USceneComponent* SpawnPoint;

    // 初期の敵数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 InitialEnemyCount = 3;

    // 各ウェーブで増える敵数
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 EnemyIncreasePerWave = 2;

    // ウェーブ間の時間（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float TimeBetweenWaves = 10.0f;

    // 現在のウェーブ数
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWave = 1;

    // 限界ウェーブ数
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 LimitWave = 5;

    // 敵全滅後に次ウェーブに進むまでの待機時間
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float TimeAfterClear = 5.0f;


    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    int32 NumberOfSpawnPoints = 5; // 配置するウェーブスポーン数

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    float MinDistanceFromPlayer = 1000.f; // プレイヤーから最低距離

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    FVector SpawnAreaMin; // スポーン可能範囲の最小座標

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    FVector SpawnAreaMax; // スポーン可能範囲の最大座標

    TArray<FVector> SpawnPoints;


private:
    FTimerHandle WaveTimerHandle;

    // 現在スポーンしている敵たちを記録
    TArray<AEnemyCharacterBase*> SpawnedEnemies;

    // ウェーブ中かどうか
    bool bWaveInProgress = false;


    
};
