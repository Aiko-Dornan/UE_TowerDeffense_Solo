// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "MyGrenadeProjectileActor.generated.h"

class ADefenseBase;
class ADefenseStructure;
class AMyHeroPlayer;
class AAllyCharacter;
class ADroneCharacter;
class AEnemyAIController;
class AEnemyCharacterBase;

UCLASS()
class TOWERDEFFENSE_FPS_API AMyGrenadeProjectileActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyGrenadeProjectileActor();

	/** 発射方向・初速度を計算 */
	void CalculateLaunchVelocity();
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Type")
	bool ExplosionFlag = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Type")
	bool PlayerFlag = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float AmountAreaAtacck = 100.0f;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* NiagaraEffect;

	// パーティクルを再生する関数
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void PlayNiagaraEffect();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void ApplyAreaDamage(float DamageAmount, float Radius);

private:
    FVector Velocity;

    

};
