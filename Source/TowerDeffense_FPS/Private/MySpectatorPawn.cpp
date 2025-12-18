// MySpectatorPawn.cpp
#include "MySpectatorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include"Components/SphereComponent.h"

AMySpectatorPawn::AMySpectatorPawn()
{
    // 完全ゴースト
    GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 高速移動
    if (UFloatingPawnMovement* Move = Cast<UFloatingPawnMovement>(GetMovementComponent()))
    {
        Move->MaxSpeed = 2000.f;
        Move->Acceleration = 4000.f;
    }
}
