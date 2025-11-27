#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DroneAIController.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ADroneAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADroneAIController();

	virtual void OnPossess(APawn* InPawn) override;

	// AI を目的地へ移動させる
	void MoveToDropPoint();

	// 移動完了イベント
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	class ADroneCharacter* ControlledDrone;
};
