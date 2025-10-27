#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

    // 敵をスポーンする関数
    void SpawnEnemy();

    // スポーン用のタイマー
    FTimerHandle SpawnTimerHandle;

public:
    // スポーンする敵のクラス（Blueprintで指定可能）
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<class AEnemyCharacterBase> EnemyClass;

    // スポーン間隔（秒）
    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnInterval = 5.0f;

    // スポーン位置（見た目用）
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    class UArrowComponent* SpawnPoint;
};
