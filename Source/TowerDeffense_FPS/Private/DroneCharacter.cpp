#include "DroneCharacter.h"
#include "AmmoBox.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ADroneCharacter::ADroneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ドローンの移動速度を変更
	GetCharacterMovement()->MaxWalkSpeed = MoveDroneSpeed;
}

void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 通常の初期化
	CurrentHealth = MaxHealth;

	if (!GetController())
	{
		SpawnDefaultController();
	}
	GetCharacterMovement()->MaxWalkSpeed = MoveDroneSpeed;
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->MoveToLocation(TargetLocation);
	}

	// 弾薬箱をスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// ★ 必ず場所と回転を渡す
	AAmmoBox* SpawnedAB = GetWorld()->SpawnActor<AAmmoBox>(
		AmmoBoxClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);


	//AAmmoBox* SpawnedAB = GetWorld()->SpawnActor<AAmmoBox>(AmmoBoxClass, SpawnParams);

	if (SpawnedAB)
	{
		const FName DroneSocketName = TEXT("DropSocket"); // スケルトンメッシュのソケット名に一致させる
		SpawnedAB->SetActorRelativeLocation(FVector::ZeroVector);
		SpawnedAB->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, DroneSocketName);
		//SpawnedAB->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, DroneSocketName);
		//SpawnedAB->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale, DroneSocketName);

	

		UStaticMeshComponent* BoxMesh = SpawnedAB->FindComponentByClass<UStaticMeshComponent>();
		if (BoxMesh)
		{
			BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		//SpawnedAB->SetActorEnableCollision(false);
	}
	/*UE_LOG(LogTemp, Warning, TEXT("AttachParent: %s"),
		*SpawnedAB->GetAttachParentActor()->GetName());*/

	

	
}

void ADroneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().Size() < 0.1f) // 動いていなければ
	{
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->MoveToLocation(TargetLocation);
		}

	}


}

void ADroneCharacter::DropAmmoBox()
{
	if (!AmmoBoxClass) return;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 足元にスポーン
	FVector SpawnPos = GetActorLocation() - FVector(0, 0, 90);

	AAmmoBox* Box = GetWorld()->SpawnActor<AAmmoBox>(AmmoBoxClass, SpawnPos, FRotator::ZeroRotator, Params);

	if (Box)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ammo Box Dropped!"));
		Destroy();
		
	}
}
void ADroneCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	TArray<AActor*> Attached;
	GetAttachedActors(Attached);

	for (AActor* Child : Attached)
	{
		if (Child && IsValid(Child))
		{
			Child->Destroy();
		}
	}
}

float ADroneCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHealth -= ActualDamage;
	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
		*GetName(), ActualDamage, CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	

	return ActualDamage;
}

// ==================== 死亡処理 ====================

void ADroneCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	Destroy();
}