// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "MyGrenadeProjectileActor.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AMyGrenadeProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyGrenadeProjectileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    /** 初速 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float LaunchSpeed;

    /** 重力スケール */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float GravityScale;

    /** 目標地点 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    FVector TargetLocation;

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

private:
    FVector Velocity;

    /** 発射方向・初速度を計算 */
    void CalculateLaunchVelocity();

};
