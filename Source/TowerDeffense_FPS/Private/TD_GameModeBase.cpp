// Fill out your copyright notice in the Description page of Project Settings.


#include "TD_GameModeBase.h"
#include"MyHeroPlayer.h"

ATD_GameModeBase::ATD_GameModeBase()
{
	DefaultPawnClass = AMyHeroPlayer::StaticClass();
}