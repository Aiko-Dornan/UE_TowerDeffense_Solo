#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneSpawner.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ADroneSpawner : public AActor
{
    GENERATED_BODY()

public:
    ADroneSpawner();

    // ドローンクラス
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    TSubclassOf<class ADroneCharacter> DroneClass;

    // スポーン間隔
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    float SpawnInterval = 5.0f;

    // ランダムスポーン範囲
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    float SpawnRadius = 6500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
    int SpawnLimit = 5;
    // スポーン処理
    void SpawnDrone();

protected:
    virtual void BeginPlay() override;

    int NowSpawn = 0;

    // スポーンの Timer
    FTimerHandle SpawnTimer;

    FVector RandomOffset;
};
