// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyCharacterBase.h"
#include "TD_GameModeBase.generated.h"
/**
 * 
 */
UCLASS()
class TOWERDEFFENSE_FPS_API ATD_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	

public:

	virtual void BeginPlay() override;

	// Constructor
	ATD_GameModeBase();

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TSubclassOf<AEnemyCharacterBase> EnemyClass;
};
