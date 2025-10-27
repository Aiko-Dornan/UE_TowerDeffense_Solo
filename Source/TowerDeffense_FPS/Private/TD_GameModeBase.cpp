#include "TD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include"EnemySpawnerWave.h"

ATD_GameModeBase::ATD_GameModeBase()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BluePrint/MyMyHeroPlayer"));
    DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void ATD_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (EnemyWaveClass)
    {
        FVector SpawnLocation(1100.f, 200.f, 100.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        AEnemySpawnerWave* SpawnedEnemy = GetWorld()->SpawnActor<AEnemySpawnerWave>(EnemyWaveClass, SpawnLocation, SpawnRotation);
        if (SpawnedEnemy)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy spawned: %s"), *SpawnedEnemy->GetName());

            // Spawn後にAIControllerをアタッチ
           /* SpawnedEnemy->SpawnDefaultController();

            if (SpawnedEnemy->GetController())
            {
                UE_LOG(LogTemp, Warning, TEXT("Enemy now has controller: %s"), *SpawnedEnemy->GetController()->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Enemy has NO controller!"));
            }*/
        }
    }
}

//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "TD_GameModeBase.h"
//#include "Engine/World.h"
//#include "Kismet/GameplayStatics.h"
//#include"MyHeroPlayer.h"
//
//
//ATD_GameModeBase::ATD_GameModeBase()
//{
//	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BluePrint/MyMyHeroPlayer"));
//	DefaultPawnClass = PlayerPawnClassFinder.Class;
//
//	//DefaultPawnClass = AMyHeroPlayer::StaticClass();
//}
//
//void ATD_GameModeBase::BeginPlay()
//{
//    Super::BeginPlay();
//
//    if (EnemyClass)
//    {
//        FVector SpawnLocation(200.f, 200.f, 100.f);
//        FRotator SpawnRotation = FRotator::ZeroRotator;
//
//        AEnemyCharacterBase* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacterBase>(EnemyClass, SpawnLocation, SpawnRotation);
//        if (SpawnedEnemy)
//        {
//            UE_LOG(LogTemp, Warning, TEXT("Enemy spawned successfully: %s"), *SpawnedEnemy->GetName());
//
//            // AIControllerが所持しているか
//            AController* Controller = SpawnedEnemy->GetController();
//            SpawnedEnemy->SpawnDefaultController();
//            if (SpawnedEnemy->GetController())
//            {
//                UE_LOG(LogTemp, Warning, TEXT("Enemy now has controller: %s"), *SpawnedEnemy->GetController()->GetName());
//            }
//            else
//            {
//                UE_LOG(LogTemp, Error, TEXT("Enemy has NO controller!"));
//            }
//
//        }
//        else
//        {
//            UE_LOG(LogTemp, Warning, TEXT("Enemy spawn failed."));
//        }
//    }
//    else
//    {
//        UE_LOG(LogTemp, Warning, TEXT("EnemyClass is null."));
//    }
//}