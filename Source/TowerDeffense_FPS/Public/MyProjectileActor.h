#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "MyProjectileActor.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AMyProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AMyProjectileActor();

protected:
	virtual void BeginPlay() override;

	FTimerHandle TimerHandle_EnableCollision;
	void EnableCollision();

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionComp;


	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 15.0f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	// íeìØémÇÃè’ìÀÇñ≥å¯Ç…Ç∑ÇÈÇ©
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bIgnoreOtherProjectiles = false;

	/*UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);*/
};
