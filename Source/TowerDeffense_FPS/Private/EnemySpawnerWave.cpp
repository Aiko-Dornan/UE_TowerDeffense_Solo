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
    // �ŏ��̃E�F�[�u���J�n
    SpawnWave();
}

void AEnemySpawnerWave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���݃E�F�[�u���ŁA�G���S�ł��Ă����玟�E�F�[�u�i�s�^�C�}�[���Z�b�g
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

        // �ŏI�E�F�[�u�Ȃ�Q�[���N���A
        if (CurrentWave >= LimitWave)
        {
            UE_LOG(LogTemp, Warning, TEXT("Final wave cleared! Game clear!"));

            ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
            if (GM)
            {
                GM->GameClear(); // GameClear() ���Ăяo��
            }

            return; // �������̃E�F�[�u�ɐi�܂Ȃ�
        }

        // �܂��c��E�F�[�u������ꍇ�͎��̃E�F�[�u��
        UE_LOG(LogTemp, Warning, TEXT("All enemies defeated! Next wave in %.1f seconds."), TimeAfterClear);
        GetWorldTimerManager().ClearTimer(WaveTimerHandle);
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeAfterClear, false);
    }

}
void AEnemySpawnerWave::SpawnWave()
{
    // ����`�F�b�N
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
        // �����_���ȃX�|�[���|�C���g��I��
        FVector SpawnLocation = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

        // �����ʒu�����炷
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
            // AI ��L����
            SpawnedEnemy->SpawnDefaultController();
            SpawnedEnemies.Add(SpawnedEnemy);
        }
    }

    bWaveInProgress = true;

    // �E�F�[�u�^�C�}�[�Z�b�g�i���Ԍo�߂Ői�s�j
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);
    GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &AEnemySpawnerWave::StartNextWave, TimeBetweenWaves, false);

    UE_LOG(LogTemp, Warning, TEXT("Wave %d started! SpawnPoints.Num = %d"), CurrentWave, SpawnPoints.Num());

    //if (!EnemyClass) return;

    //UWorld* World = GetWorld();
    //if (!World) return;

    //// �Â��G���X�g���N���A
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

    //// �ʏ�E�F�[�u�^�C�}�[���Z�b�g�i���ԂŐi�ޗp�j
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

    //    // �G�����������炵�ăX�|�[���i�d�Ȃ�Ȃ��悤�Ɂj
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
    //        // AI ��L�����i�d�v�I�j
    //        SpawnedEnemy->SpawnDefaultController();
    //    }
    //}

    //// ���̃E�F�[�u���Z�b�g
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
    // ���łɍŏI�E�F�[�u�Ȃ牽�����Ȃ�
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
        if (IsValid(Enemy)) // nullptr �� PendingKill �łȂ���ΐ���
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

    //    //// �v���C���[�Ƃ̋����`�F�b�N
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
        UE_LOG(LogTemp, Warning, TEXT("�\���ȋ�����ۂĂ�X�|�[���|�C���g�𐶐��ł��܂���ł���"));
    }*/

    //SpawnPoints.Empty();

    //APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    //if (!PlayerPawn) return;

    //int32 SpawnedCount = 0;
    //int32 MaxAttempts = 100; // �������[�v�h�~
    //int32 Attempts = 0;

    //while (SpawnedCount < NumberOfSpawnPoints && Attempts < MaxAttempts)
    //{
    //    Attempts++;

    //    // �����_���ʒu����
    //    float RandX = FMath::FRandRange(SpawnAreaMin.X, SpawnAreaMax.X);
    //    float RandY = FMath::FRandRange(SpawnAreaMin.Y, SpawnAreaMax.Y);
    //    float RandZ = FMath::FRandRange(SpawnAreaMin.Z, SpawnAreaMax.Z);
    //    FVector CandidateLocation(RandX, RandY, RandZ);

    //    // �v���C���[�Ƃ̋������`�F�b�N
    //    if (FVector::Dist(PlayerPawn->GetActorLocation(), CandidateLocation) >= MinDistanceFromPlayer)
    //    {
    //        SpawnPoints.Add(CandidateLocation);
    //        SpawnedCount++;
    //    }
    //}

    //if (SpawnedCount < NumberOfSpawnPoints)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("�\���ȋ�����ۂĂ�X�|�[���|�C���g�𐶐��ł��܂���ł���"));
    //}
}