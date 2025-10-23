// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_GameModeBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include"MyHeroPlayer.h"


ATD_GameModeBase::ATD_GameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BluePrint/MyMyHeroPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	//DefaultPawnClass = AMyHeroPlayer::StaticClass();
}

void ATD_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    if (EnemyClass)
    {
        FVector SpawnLocation(200.f, 200.f, 100.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        GetWorld()->SpawnActor<AEnemyCharacterBase>(EnemyClass, SpawnLocation, SpawnRotation);
    }
}