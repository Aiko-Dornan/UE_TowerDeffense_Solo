#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "AllyCharacter.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAllyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ===== AI�ݒ� =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float MaintainDistance = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AcceptableRadius = 50.0f;

    UPROPERTY(BlueprintReadWrite, Category = "AI")
    AActor* TargetEnemy;

    // ===== ���� =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
    AWeaponBase* EquippedWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 0.5f;

    FTimerHandle FireTimerHandle;

    // �G�ĒT���p�^�C�}�[�i�����ǉ��I�j
    FTimerHandle TargetUpdateTimer;

    // ===== �����֐� =====
    void HandleFire();
    void MoveTowards(AActor* Target, float DesiredDistance);
    void MoveAwayFrom(AActor* Target, float DesiredDistance);
    void StopMovement();
    void FaceTarget(AActor* Target);
    void FindNearestEnemy(); // Tick���ƂɌĂ΂��G�T��

public:
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetTargetEnemy(AActor* NewTarget);
};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "WeaponBase.h"
//#include "AllyCharacter.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaTime) override;
//
//    // ===== AI�Ǐ] =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MaintainDistance = 600.0f; // �G�ƕۂ���������
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AcceptableRadius = 50.0f; // ���B�덷
//
//    UPROPERTY(BlueprintReadWrite, Category = "AI")
//    AActor* TargetEnemy; // �ǔ�����G
//
//    // ===== ���� =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireRange = 3000.0f;
//
//    // �ˌ��Ԋu
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    FTimerHandle FireTimerHandle;
//
//    void HandleFire();
//
//    void MoveTowards(AActor* Target, float DesiredDistance);
//    void MoveAwayFrom(AActor* Target, float DesiredDistance);
//    void StopMovement();
//
//public:
//    UFUNCTION(BlueprintCallable, Category = "AI")
//    void SetTargetEnemy(AActor* NewTarget);
//
//};
//
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "WeaponBase.h"
//#include "AllyCharacter.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyCharacter : public ACharacter
//{
//    GENERATED_BODY()
//
//public:
//    AAllyCharacter();
//
//protected:
//    virtual void BeginPlay() override;
//    
//
//    // ==== ����֘A ====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    FName WeaponAttachSocketName = "WeaponSocket";
//
//    void EquipWeapon(AWeaponBase* NewWeapon);
//
//    // ==== �U���֘A ====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireInterval = 0.5f;
//
//    FTimerHandle FireTimerHandle;
//
//    void FireAtTarget();
//
//    AActor* FindNearestEnemy();
//
//public:
//    virtual void Tick(float DeltaTime) override;
//};
