#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveWidget.h"  // WaveWidget を追加
#include "EnemyCharacterBase.h"
#include"AmmoDisplayWidget.h"
#include"MyHeroPlayer.h"
#include"TD_GameInstance.h"
#include "EnemySpawnerWave.generated.h"

// ウェーブ更新通知イベント
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveChanged, int32, CurrentWave, int32, MaxWave);
// Waveが変化したときに通知するデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveChanged, int32, CurrentWave);


UCLASS()
class TOWERDEFFENSE_FPS_API AEnemySpawnerWave : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawnerWave();

    // 敵が全滅しているか確認
    bool AreAllEnemiesDead() const;

    void NotifyEnemyDestroyed(AEnemyCharacterBase* DeadEnemy);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 敵を実際にスポーンする関数
    void SpawnWave();

    void GenerateRandomSpawnPoints();

    // 次のウェーブへ進行
    void StartNextWave();

    UPROPERTY()
    class AMyHeroPlayer* MHP;

public:
    // 敵のクラス1Base
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClassTypeB[5];

    // 敵のクラス2Grenade
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClassTypeG[5];

    // 敵のクラス3Tank
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClassTypeT[5];

    // 敵のクラス4Kamikaze
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClassTypeK[5];

    UPROPERTY(EditAnywhere, Category = "Boss")
    TSubclassOf<AEnemyCharacterBase> BossEnemyClass[5];

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 LimitWave = 5;

    UPROPERTY()
    UWaveWidget* WaveWidgetInstance; // WaveWidget のインスタンス

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> WaveWidgetClass; // WaveWidget の Blueprint クラス

    // Wave変化時に呼び出す
    UPROPERTY(BlueprintAssignable, Category = "Wave")
    FOnWaveChanged OnWaveChanged;

    //UPROPERTY(BlueprintAssignable, Category = "Wave")
    //FOnWaveChanged OnWaveChanged; //  ウィジェット側が購読できるイベント


    // 敵全滅後に次ウェーブに進むまでの待機時間
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float TimeAfterClear = 5.0f;


    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    int32 NumberOfSpawnPoints = 5; // 配置するウェーブスポーン数

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    float MinDistanceFromPlayer = 1000.f; // プレイヤーから最低距離

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    float MinDistanceFromBase = 1000.f; // 基地から最低距離

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    float MaxDistanceFromBase = 1000.f; // 基地から最大距離

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

    bool all_spawn = false;
    
    int RandomInt = 0;

    UPROPERTY(VisibleAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClass;

    FTimerHandle GameClearHandle;
    void GameClearAct();


};
