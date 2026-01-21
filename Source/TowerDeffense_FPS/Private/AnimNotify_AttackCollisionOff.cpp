// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackCollisionOff.h"
#include "EnemyCharacterBase.h"

// AnimNotify_AttackCollisionOff.cpp
void UAnimNotify_AttackCollisionOff::Notify(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation)
{
    if (AEnemyCharacterBase* Enemy =
        Cast<AEnemyCharacterBase>(MeshComp->GetOwner()))
    {
        Enemy->AttackCollision->SetCollisionEnabled(
            ECollisionEnabled::NoCollision);
    }
}
