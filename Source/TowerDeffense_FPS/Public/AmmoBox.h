#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include"DropPointActor.h"
#include "NiagaraSystem.h"
#include "AmmoBox.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API AAmmoBox : public AActor
{
	UPROPERTY()
	class AMyHeroPlayer* OverlappingPlayer;

	GENERATED_BODY()

public:
	AAmmoBox();

	void PlayNiagaraEffect();
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* RangeCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GiveAmmoAmount = 60.0f;   // 補充する弾薬量

	// プレイヤーが範囲内に居るか
	bool bPlayerInRange = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsParentDrone = true;

	/*UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);*/

	UFUNCTION()
	void OnRangeBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnRangeEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// プレイヤーがキー押した時に呼ぶ
	void TryGiveAmmo(class AWeaponBase* Weapon);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY()
	ADropPointActor* LinkedDropPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* NiagaraEffect;

private:
	bool IsAmmoGet = false;

};
