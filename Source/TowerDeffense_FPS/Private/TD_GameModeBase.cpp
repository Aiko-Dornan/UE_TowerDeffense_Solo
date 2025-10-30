#include "TD_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include"EnemySpawnerWave.h"
#include"DefenseBase.h"
#include "Blueprint/UserWidget.h"


ATD_GameModeBase::ATD_GameModeBase()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BluePrint/MyMyHeroPlayer"));
    DefaultPawnClass = PlayerPawnClassFinder.Class;
}

void ATD_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (DefenseBaseClass)
    {
        FVector SpawnLocation(0.0f, 14520.f, 0.0f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        ADefenseBase* SpawnedBase = GetWorld()->SpawnActor<ADefenseBase>(DefenseBaseClass, SpawnLocation, SpawnRotation);
    }

    if (AllyClass)
    {
        FVector SpawnLocation(-800.0f, 11000.f, 0.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        AAllyCharacter* SpawnedBase = GetWorld()->SpawnActor<AAllyCharacter>(AllyClass, SpawnLocation, SpawnRotation);
    }


    if (EnemyWaveClass)
    {
        FVector SpawnLocation(0.0f, 14520.f, 100.f);
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

    if (WaveWidgetClass)
    {
        WaveWidgetInstance = CreateWidget<UWaveWidget>(GetWorld(), WaveWidgetClass);
        if (WaveWidgetInstance)
        {
            WaveWidgetInstance->AddToViewport();
        }
    }

    // EnemySpawnerWaveを探してバインド
    TArray<AActor*> FoundSpawners;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnerWave::StaticClass(), FoundSpawners);

    if (FoundSpawners.Num() > 0)
    {
        EnemySpawnerRef = Cast<AEnemySpawnerWave>(FoundSpawners[0]);
        if (EnemySpawnerRef && WaveWidgetInstance)
        {
            EnemySpawnerRef->OnWaveChanged.AddDynamic(WaveWidgetInstance, &UWaveWidget::UpdateWaveText);
        }
    }

}

void ATD_GameModeBase::GameOver()
{
    UE_LOG(LogTemp, Error, TEXT("=== GAME OVER ==="));

    // 時間を停止
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

    // プレイヤー操作を無効化
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->SetPause(true);
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
        PC->bShowMouseCursor = true; // UI操作用にマウスを出す
    }

    //  「Game Over」を画面に出す（シンプル版）
   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GAME OVER"));

    // UI（Widget）を表示したい場合
    
    if (GameOverWidgetClass)
    {
        UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();
        }
    }
    
}

void ATD_GameModeBase::GameClear()
{
    UE_LOG(LogTemp, Warning, TEXT("=== GAME CLEAR! ==="));
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
        PC->bShowMouseCursor = true;
    }

    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GAME CLEAR!"));

    // BlueprintでUIを出したいならここに追加
    
    if (GameClearWidgetClass)
    {
        UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), GameClearWidgetClass);
        if (Widget) Widget->AddToViewport();
    }
    
}

void ATD_GameModeBase::OnEnemyDestroyed()
{
    // スポーナーが存在するか確認
    if (!EnemySpawnerRef) return;

    int32 CurrentWave = EnemySpawnerRef->CurrentWave;
    int32 LimitWave = EnemySpawnerRef->LimitWave;
    bool bAllDead = EnemySpawnerRef->AreAllEnemiesDead();

    UE_LOG(LogTemp, Warning, TEXT("Enemy destroyed. Wave %d/%d, AllDead=%d"),
        CurrentWave, LimitWave, bAllDead ? 1 : 0);

    // 条件：最終ウェーブ & 全滅
    if (bAllDead && CurrentWave >= LimitWave)
    {
        GameClear();
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