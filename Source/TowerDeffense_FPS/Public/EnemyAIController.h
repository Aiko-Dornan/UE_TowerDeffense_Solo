#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    APawn* PlayerPawn;
};
