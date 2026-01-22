// AnimNotify_EnemyAttackHit.cpp
#include "AnimNotify_EnemyAttackHit.h"
#include "EnemyCharacterBase.h"

void UAnimNotify_EnemyAttackHit::Notify(USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    if (AEnemyCharacterBase* Enemy =
        Cast<AEnemyCharacterBase>(MeshComp->GetOwner()))
    {
        Enemy->AttackCollision->SetCollisionEnabled(
            ECollisionEnabled::QueryOnly);

        Enemy->HitActors.Empty();

       
    }

    //AEnemyCharacterBase* Enemy =
    //    Cast<AEnemyCharacterBase>(MeshComp->GetOwner());

    //if (Enemy)
    //{
    //    Enemy->OnAttackHit(); // © “G‘¤‚Åƒ_ƒ[ƒWˆ—‚ğŒÄ‚Ô
    //}
}
