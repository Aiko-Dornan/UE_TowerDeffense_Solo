#pragma once

#include "CoreMinimal.h"
#include"EnemyCharacterBase.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

protected:
    //AEnemyAIController();
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;

private:
    APawn* PlayerPawn;
    AActor* TargetActor;
    AEnemyCharacterBase* CurrentEnemy;

    // 現在のターゲット
    UPROPERTY()
    AActor* CurrentTarget;

    // ターゲットを更新して移動開始
    //void UpdateTargetAndMove();
};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "AIController.h"
//#include "EnemyAIController.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyAIController : public AAIController
//{
//    GENERATED_BODY()
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaSeconds) override;
//
//private:
//    APawn* PlayerPawn;
//};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "AIController.h"
//#include "EnemyAIController.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AEnemyAIController : public AAIController
//{
//    GENERATED_BODY()
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaSeconds) override;
//
//private:
//    APawn* PlayerPawn;
//};
