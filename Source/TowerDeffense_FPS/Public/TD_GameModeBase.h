// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyCharacterBase.h"
#include"EnemySpawnerWave.h"
#include"DefenseBase.h"
#include"AllyCharacter.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameClearWidgetClass;


    UFUNCTION(BlueprintCallable)
    void GameClear();

    // �X�|�[�i�[�ւ̎Q��
    AEnemySpawnerWave* EnemySpawnerRef;

    // �c��̓G����ǐՁi�K�v�ɉ����Ďg���j
    int32 AliveEnemies = 0;

    

    // �G�̎��S��ʒm����
    void OnEnemyDestroyed();

    // �Q�[���I�[�o�[����
    UFUNCTION(BlueprintCallable)
    void GameOver();


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
