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

    // �G�����ۂɃX�|�[������֐�
    void SpawnWave();

    void GenerateRandomSpawnPoints();

    // ���̃E�F�[�u�֐i�s
    void StartNextWave();

    // �G���S�ł��Ă��邩�m�F
    bool AreAllEnemiesDead() const;

public:
    // �G�̃N���X
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<AEnemyCharacterBase> EnemyClass;

    // �X�|�[���ʒu
    UPROPERTY(EditAnywhere, Category = "Spawn")
    USceneComponent* SpawnPoint;

    // �����̓G��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 InitialEnemyCount = 3;

    // �e�E�F�[�u�ő�����G��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 EnemyIncreasePerWave = 2;

    // �E�F�[�u�Ԃ̎��ԁi�b�j
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float TimeBetweenWaves = 10.0f;

    // ���݂̃E�F�[�u��
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWave = 1;

    // ���E�E�F�[�u��
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 LimitWave = 5;

    // �G�S�Ō�Ɏ��E�F�[�u�ɐi�ނ܂ł̑ҋ@����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float TimeAfterClear = 5.0f;


    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    int32 NumberOfSpawnPoints = 5; // �z�u����E�F�[�u�X�|�[����

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    float MinDistanceFromPlayer = 1000.f; // �v���C���[����Œ዗��

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    FVector SpawnAreaMin; // �X�|�[���\�͈͂̍ŏ����W

    UPROPERTY(EditAnywhere, Category = "WaveSpawn")
    FVector SpawnAreaMax; // �X�|�[���\�͈͂̍ő���W

    TArray<FVector> SpawnPoints;


private:
    FTimerHandle WaveTimerHandle;

    // ���݃X�|�[�����Ă���G�������L�^
    TArray<AEnemyCharacterBase*> SpawnedEnemies;

    // �E�F�[�u�����ǂ���
    bool bWaveInProgress = false;


    
};
