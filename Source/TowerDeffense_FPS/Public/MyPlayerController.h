// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyHeroPlayer.h"
#include "MyPlayerController.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    AMyPlayerController();

   

protected:
    AMyHeroPlayer* m_pAMyCharacter;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    void MoveForward_Axis(float axisValue);
    void MoveRight_Axis(float axisValue);
	
    void StartJump_Input();
    void StopJump_Input();

};
