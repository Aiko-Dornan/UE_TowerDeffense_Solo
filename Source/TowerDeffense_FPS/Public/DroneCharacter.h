#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"DropPointActor.h"
#include"AmmoDisplayWidget.h"
#include"MyHeroPlayer.h"
#include "DroneCharacter.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	ADroneCharacter();

	virtual void Tick(float DeltaTime) override;

	// 目的地
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FVector TargetLocation;

	// 弾薬箱クラス
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TSubclassOf<class AAmmoBox> AmmoBoxClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MoveDroneSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float MaxHealth = 100.0f;

	UPROPERTY()
	TArray<AActor*> TargetActors;

	UPROPERTY()
	class AMyHeroPlayer* MHP;

	/*UPROPERTY()
	UAmmoDisplay* AmmoD;*/

	int32 CurrentTargetIndex = 0;

	// ====================================
	// ここに追加：現在のターゲットポイント
	ADropPointActor* CurrentTarget = nullptr;

	void MoveToNextTarget();

	// 到着後に呼ばれる
	void DropAmmoBox();

	void RefreshTargetList();

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

private:
	

	bool bIsDead = false;

	float CurrentHealth;
};
