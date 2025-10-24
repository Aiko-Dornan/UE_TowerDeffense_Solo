#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacterBase.generated.h"

class AEnemyAIController;

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacterBase();

    virtual void Tick(float DeltaTime) override;

    void PerformAttack();

protected:
    virtual void BeginPlay() override;

public:
    // �U���͈�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackRange;

    // �U���N�[���_�E��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AttackCooldown;

private:
    bool bCanAttack = true;
    FTimerHandle AttackCooldownTimerHandle;

    void ResetAttack();
};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//protected:
//    virtual void BeginPlay() override;
//
//public:
//    virtual void Tick(float DeltaTime) override;
//
//    // �U�������iAIController ����ĂԂ��� public �Ɂj
//    void PerformAttack();
//
//    // �U���͈�
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackRange;
//
//    // �U���N�[���_�E���i�b�j
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AttackCooldown;
//
//    // �ړ����x
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MoveSpeed;
//
//private:
//    bool bCanAttack;
//    FTimerHandle AttackCooldownTimerHandle;
//
//    void ResetAttack();
//};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//protected:
//    virtual void BeginPlay() override;
//
//    UPROPERTY()
//    APawn* PlayerPawn;
//
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackRange = 150.0f;
//
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackCooldown = 2.0f;
//
//    bool bCanAttack = true;
//
//    FTimerHandle AttackCooldownTimerHandle;
//
//    void PerformAttack();
//    void ResetAttack();
//
//
//
//public:
//    virtual void Tick(float DeltaTime) override;
//};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacterBase.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyCharacterBase : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AEnemyCharacterBase();
//
//protected:
//    virtual void BeginPlay() override;
//
//    // �^�[�Q�b�g(�v���C���[)
//    UPROPERTY()
//    APawn* PlayerPawn;
//
//    // �U���͈�
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackRange;
//
//    // �U���N�[���_�E���i�b�j
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float AttackCooldown;
//
//    // �ړ����x
//    UPROPERTY(EditAnywhere, Category = "AI")
//    float MoveSpeed;
//
//    bool bCanAttack;
//
//    FTimerHandle AttackCooldownTimerHandle;
//
//    // �U������
//    void PerformAttack();
//
//    void ResetAttack();
//
//    // �ړ�����
//    void MoveToPlayer();
//
//public:
//    virtual void Tick(float DeltaTime) override;
//};
