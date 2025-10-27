#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacterBase.generated.h"

class AEnemyAIController;
class AMyHeroPlayer;

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    void PerformAttack();

    

protected:
    virtual void BeginPlay() override;
    void ResetAttack();
    void Die();  // éÄñSèàóù

public:
    // çUåÇê›íË
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackDamage = 10.0f;

    // === HPä÷òA ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
    float CurrentHealth;



private:
    bool bCanAttack = true;
    bool bIsDead = false;

    FTimerHandle AttackCooldownTimerHandle;
    AMyHeroPlayer* PlayerCharacter;
};
