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

    // �G���X�|�[������֐�
    void SpawnEnemy();

    // �X�|�[���p�̃^�C�}�[
    FTimerHandle SpawnTimerHandle;

public:
    // �X�|�[������G�̃N���X�iBlueprint�Ŏw��\�j
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TSubclassOf<class AEnemyCharacterBase> EnemyClass;

    // �X�|�[���Ԋu�i�b�j
    UPROPERTY(EditAnywhere, Category = "Spawner")
    float SpawnInterval = 5.0f;

    // �X�|�[���ʒu�i�����ڗp�j
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    class UArrowComponent* SpawnPoint;
};
