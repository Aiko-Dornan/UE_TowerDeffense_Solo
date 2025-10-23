#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacterBase.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

protected:
    virtual void BeginPlay() override;

    // �^�[�Q�b�g(�v���C���[)
    UPROPERTY()
    APawn* PlayerPawn;

    // �U���͈�
    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange;

    // �U���N�[���_�E���i�b�j
    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackCooldown;

    // �ړ����x
    UPROPERTY(EditAnywhere, Category = "AI")
    float MoveSpeed;

    bool bCanAttack;

    FTimerHandle AttackCooldownTimerHandle;

    // �U������
    void PerformAttack();

    void ResetAttack();

    // �ړ�����
    void MoveToPlayer();

public:
    virtual void Tick(float DeltaTime) override;
};
