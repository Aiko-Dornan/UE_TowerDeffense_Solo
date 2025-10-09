// Copyright Epic Games, Inc. All Rights Reserved.

#include "TowerDeffense_FPSPickUpComponent.h"

UTowerDeffense_FPSPickUpComponent::UTowerDeffense_FPSPickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UTowerDeffense_FPSPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UTowerDeffense_FPSPickUpComponent::OnSphereBeginOverlap);
}

void UTowerDeffense_FPSPickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	ATowerDeffense_FPSCharacter* Character = Cast<ATowerDeffense_FPSCharacter>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
