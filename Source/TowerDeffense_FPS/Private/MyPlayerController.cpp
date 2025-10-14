// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
    : APlayerController()
    , m_pAMyCharacter(nullptr)
{

}

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();
    APawn* pPawn = GetPawn();
    ensure(pPawn != nullptr);
    m_pAMyCharacter = Cast<AMyHeroPlayer>(pPawn);
    ensure(m_pAMyCharacter != nullptr);
}

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAxis("MoveForward", this, &AMyPlayerController::MoveForward_Axis);
}

void AMyPlayerController::MoveForward_Axis(float axisValue)
{
    check(m_pAMyCharacter != nullptr);
    m_pAMyCharacter->MoveForward(axisValue);
}

void AMyPlayerController::MoveRight_Axis(float axisValue)
{
    check(m_pAMyCharacter != nullptr);
    m_pAMyCharacter->MoveRight(axisValue);
}