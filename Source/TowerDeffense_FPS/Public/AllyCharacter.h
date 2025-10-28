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

    // ===== AIê›íË =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float MaintainDistance = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float AcceptableRadius = 50.0f;

    UPROPERTY(BlueprintReadWrite, Category = "AI")
    AActor* TargetEnemy;

    // ===== ïêäÌ =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
    AWeaponBase* EquippedWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireRange = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FireInterval = 0.5f;

    FTimerHandle FireTimerHandle;

    // ìGçƒíTçıópÉ^ÉCÉ}Å[ÅiÇ±ÇÍÇí«â¡ÅIÅj
    FTimerHandle TargetUpdateTimer;

    // ===== ì‡ïîä÷êî =====
    void HandleFire();
    void MoveTowards(AActor* Target, float DesiredDistance);
    void MoveAwayFrom(AActor* Target, float DesiredDistance);
    void StopMovement();
    void FaceTarget(AActor* Target);
    void FindNearestEnemy(); // TickÇ≤Ç∆Ç…åƒÇŒÇÍÇÈìGíTçı

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
//    // ===== AIí«è] =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float MaintainDistance = 600.0f; // ìGÇ∆ï€ÇøÇΩÇ¢ãóó£
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
//    float AcceptableRadius = 50.0f; // ìûíBåÎç∑
//
//    UPROPERTY(BlueprintReadWrite, Category = "AI")
//    AActor* TargetEnemy; // í«îˆÇ∑ÇÈìG
//
//    // ===== ïêäÌ =====
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
//    TSubclassOf<AWeaponBase> WeaponClass;
//
//    UPROPERTY(BlueprintReadWrite, Category = "Weapon")
//    AWeaponBase* EquippedWeapon;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
//    float FireRange = 3000.0f;
//
//    // éÀåÇä‘äu
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
//    // ==== ïêäÌä÷òA ====
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
//    // ==== çUåÇä÷òA ====
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
