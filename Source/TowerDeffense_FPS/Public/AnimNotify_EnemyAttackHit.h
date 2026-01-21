// Fill out your copyright notice in the Description page of Project Settings.

// AnimNotify_EnemyAttackHit.h
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnemyAttackHit.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UAnimNotify_EnemyAttackHit : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation) override;
};
