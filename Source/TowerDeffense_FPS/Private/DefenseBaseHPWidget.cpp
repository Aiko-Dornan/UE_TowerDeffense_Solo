// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenseBaseHPWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UDefenseBaseHPWidget::UpdateHP(float CurrentHP, float MaxHP)
{
    if (HPBar)
    {
        float Ratio = CurrentHP / MaxHP;
        HPBar->SetPercent(Ratio);
    }
}