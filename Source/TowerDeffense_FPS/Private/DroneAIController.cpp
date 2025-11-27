#include "DroneAIController.h"
#include "DroneCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h" // Å© í«â¡

ADroneAIController::ADroneAIController()
{
}

void ADroneAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledDrone = Cast<ADroneCharacter>(InPawn);
	MoveToDropPoint();
}

void ADroneAIController::MoveToDropPoint()
{
	if (!ControlledDrone) return;

	MoveToLocation(ControlledDrone->TargetLocation);
}

// OnMoveCompleted ÇÃå^ÇèCê≥
void ADroneAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (!ControlledDrone) return;

	// UE5.5 Ç≈ÇÕ Success îªíËÇÕ Result.IsSuccess()
	if (Result.IsSuccess())
	{
		ControlledDrone->DropAmmoBox();
	}
}
