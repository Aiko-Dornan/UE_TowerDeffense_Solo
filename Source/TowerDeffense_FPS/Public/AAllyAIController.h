#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "AAllyAIController.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AAllyAIController : public AAIController
{
    GENERATED_BODY()
public:
       //AAllyAIController();
protected:
    virtual void OnPossess(APawn* InPawn) override;
   // virtual void BeginPlay() override;
};



//#pragma once
//
//#include "CoreMinimal.h"
//#include "AIController.h"
//#include "AAllyAIController.generated.h"
//
//class AAllyCharacter;
//class AEnemyCharacterBase;
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyAIController : public AAIController
//{
//    GENERATED_BODY()
//
//public:
//    AAllyAIController();
//
//protected:
//    virtual void OnPossess(APawn* InPawn) override;
//    virtual void Tick(float DeltaSeconds) override;
//
//private:
//    UPROPERTY()
//    AAllyCharacter* ControlledAlly;
//
//    UPROPERTY()
//    AEnemyCharacterBase* TargetEnemy;
//
//    void UpdateTargetEnemy();
//    void MaintainCombatDistance(float DeltaSeconds);
//    void FaceEnemy(float DeltaSeconds);
//
//    FTimerHandle TargetUpdateTimer;
//};


//#pragma once
//
//#include "CoreMinimal.h"
//#include "AIController.h"
//#include "AAllyAIController.generated.h"
//
//class AAllyCharacter;
//class AEnemyCharacterBase;
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API AAllyAIController : public AAIController
//{
//    GENERATED_BODY()
//
//public:
//    AAllyAIController();
//
//protected:
//    virtual void BeginPlay() override;
//    virtual void Tick(float DeltaSeconds) override;
//
//private:
//    UPROPERTY()
//    AAllyCharacter* ControlledAlly;
//
//    UPROPERTY()
//    AEnemyCharacterBase* TargetEnemy;
//
//    void UpdateTargetEnemy();
//    void MaintainCombatDistance(float DeltaSeconds);
//    void FaceEnemy(float DeltaSeconds);
//
//    FTimerHandle TargetUpdateTimer;
//};
