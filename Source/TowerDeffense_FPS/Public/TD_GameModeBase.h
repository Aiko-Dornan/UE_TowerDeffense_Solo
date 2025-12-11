// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyCharacterBase.h"
#include"EnemySpawnerWave.h"
#include"DefenseBase.h"
#include"AllyCharacter.h"
#include"DroneSpawner.h"
#include "WaveWidget.h"
#include "DefenseBaseHPWidget.h"
#include "TD_GameModeBase.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ATD_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATD_GameModeBase();

    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    TSubclassOf<AEnemyCharacterBase> EnemyClass;
    UPROPERTY(EditDefaultsOnly, Category = "EnemyWave")
    TSubclassOf<AEnemySpawnerWave> EnemyWaveClass;
    UPROPERTY(EditDefaultsOnly, Category = "DefenseBase")
    TSubclassOf<ADefenseBase> DefenseBaseClass;
    UPROPERTY(EditDefaultsOnly, Category = "Ally")
    TSubclassOf<AAllyCharacter> AllyClass;
    UPROPERTY(EditDefaultsOnly, Category = "Drone")
    TSubclassOf<ADroneSpawner> DroneClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameClearWidgetClass;

    UPROPERTY()
    UDefenseBaseHPWidget* DefenseBaseHPWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> DefenseBaseHPWidgetClass;

    UFUNCTION(BlueprintCallable)
    void GameClear();

    // スポーナーへの参照
    AEnemySpawnerWave* EnemySpawnerRef;

    // 残りの敵数を追跡（必要に応じて使う）
    int32 AliveEnemies = 0;

    

    // 敵の死亡を通知する
    void OnEnemyDestroyed();

    // ゲームオーバー処理
    UFUNCTION(BlueprintCallable)
    void GameOver();

    //Wave数表示
    UPROPERTY()
    UWaveWidget* WaveWidgetInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UWaveWidget> WaveWidgetClass;

};

//#include "CoreMinimal.h"
//#include "GameFramework/GameModeBase.h"
//#include "EnemyCharacterBase.h"
//#include "TD_GameModeBase.generated.h"
///**
// * 
// */
//UCLASS()
//class TOWERDEFFENSE_FPS_API ATD_GameModeBase : public AGameModeBase
//{
//	GENERATED_BODY()
//	
//	
//
//public:
//
//	virtual void BeginPlay() override;
//
//	// Constructor
//	ATD_GameModeBase();
//
//	UPROPERTY(EditAnywhere, Category = "Enemy")
//TSubclassOf<AEnemyCharacterBase> EnemyClass;
//};
