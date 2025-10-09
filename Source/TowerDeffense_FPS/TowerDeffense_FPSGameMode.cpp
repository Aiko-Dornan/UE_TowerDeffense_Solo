// Copyright Epic Games, Inc. All Rights Reserved.

#include "TowerDeffense_FPSGameMode.h"
#include "TowerDeffense_FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATowerDeffense_FPSGameMode::ATowerDeffense_FPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
