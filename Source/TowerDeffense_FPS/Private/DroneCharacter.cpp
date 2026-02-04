#include "DroneCharacter.h"
#include "AmmoBox.h"
#include"DropPointActor.h"
#include "AIController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include"AmmoDisplayWidget.h"
#include"MyHeroPlayer.h"

ADroneCharacter::ADroneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ドローンの移動速度を変更
	GetCharacterMovement()->MaxWalkSpeed = MoveDroneSpeed;

	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECC_GameTraceChannel3, // BlockVolumeOnly に割り当てた Channel
		ECR_Ignore
	);

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
		//AICon->MoveToLocation(TargetLocation);
	}

	TArray<AActor*> FoundPlayer;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMyHeroPlayer::StaticClass(), FoundPlayer);
	MHP = Cast<AMyHeroPlayer>(FoundPlayer[0]);

	// 弾薬箱をスポーン
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//  必ず場所と回転を渡す
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

		//  タグ "TargetPoint" を持つアクタをすべて取得
	
	

	TArray<AActor*> AllTargets;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DropTargetPoint"), AllTargets);

	for (AActor* Actor : AllTargets)
	{
		ADropPointActor* Point = Cast<ADropPointActor>(Actor);
		if (Point && !Point->bAlreadyDropped)
		{
			TargetActors.Add(Point);
		}
	}

	// ソートしたい場合（距離順など）はここで可能
	// TargetActors.Sort([&](AActor* A, AActor* B){
	//     return (A->GetActorLocation() - GetActorLocation()).Size() <
	//            (B->GetActorLocation() - GetActorLocation()).Size();
	// });

	MoveToNextTarget();

	if (MHP!=nullptr&&MHP->AmmoWidget!=nullptr)
	{
		MHP->AmmoWidget->UpdateDroneText(1);
	}

	
}

void ADroneCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActors.Num() == 0) return;
	if (GetVelocity().Size() < 0.1f) // 動いていなければ
	{
		/*if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->MoveToLocation(TargetLocation);
		}*/
		MoveToNextTarget();

	}
	
	//TArray<AActor*> AllTargets;
	////UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DropTargetPoint"), AllTargets);

	//for (AActor* Actor : AllTargets)
	//{
	//	ADropPointActor* Point = Cast<ADropPointActor>(Actor);
	//	if (Point && !Point->bAlreadyDropped)
	//	{
	//		TargetActors.Add(Point);
	//	}
	//	else
	//	{
	//		TargetActors.Remove(Point);
	//	}
	//}
	

}

void ADroneCharacter::MoveToNextTarget()
{
	if (TargetActors.Num() == 0) return;

	CurrentTarget = Cast<ADropPointActor>(TargetActors[CurrentTargetIndex]);

	if (CurrentTarget == nullptr || CurrentTarget->bAlreadyDropped)
	{
		TArray<AActor*> AllTargets;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DropTargetPoint"), AllTargets);

		for (AActor* Actor : AllTargets)
		{
			ADropPointActor* Point = Cast<ADropPointActor>(Actor);
			if (Point && !Point->bAlreadyDropped)
			{
				TargetActors.Add(Point);
			}
			else
			{
				TargetActors.Remove(Point);
			}
		}
	


		return;
	}

	if (CurrentTargetIndex >= TargetActors.Num())
	{
		DropAmmoBox();
		return;
	}

	

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->MoveToLocation(CurrentTarget->GetActorLocation());
	}
}

void ADroneCharacter::RefreshTargetList()
{
	TargetActors.Empty();

	TArray<AActor*> AllTargets;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("DropTargetPoint"), AllTargets);

	for (AActor* Actor : AllTargets)
	{
		ADropPointActor* Point = Cast<ADropPointActor>(Actor);
		if (Point && !Point->bAlreadyDropped)
		{
			TargetActors.Add(Point);
		}
	}

	// リストが変わったのでインデックスをリセット
	CurrentTargetIndex = 0;
}


//void ADroneCharacter::MoveToNextTarget()
//{
//	if (TargetActors.Num() == 0) return;
//
//	if (CurrentTargetIndex >= TargetActors.Num())
//	{
//		// 全部回った → ドロップして終了等
//		DropAmmoBox();
//		return;
//	}
//
//	AActor* Target = TargetActors[CurrentTargetIndex];
//	if (!Target) return;
//
//	if (AAIController* AICon = Cast<AAIController>(GetController()))
//	{
//		AICon->MoveToLocation(Target->GetActorLocation());
//	}
//}

void ADroneCharacter::DropAmmoBox()
{
	if (CurrentTarget)
	{
		CurrentTarget->bAlreadyDropped = true;

		UE_LOG(LogTemp, Warning, TEXT("Target %s received ammo box!"),
			*CurrentTarget->GetName());
	}

	// 既存のドロップ処理
	if (!AmmoBoxClass) return;

	FVector SpawnPos = GetActorLocation() - FVector(0, 0, 90);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AAmmoBox* Box = GetWorld()->SpawnActor<AAmmoBox>(AmmoBoxClass, SpawnPos, FRotator::ZeroRotator, Params);

	for (int32 i = 0; i < 3; i++)
	{

		// ランダムなスポーンポイントを選択
		FVector SpawnLocation = Box->GetActorLocation();

		// 少し位置をずらす
		SpawnLocation += FVector(FMath::RandRange(-300, 300), FMath::RandRange(-300, 300), 0);

		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		RandomInt = FMath::RandRange(0, 6);

		
			ItemClass = DropItem[RandomInt];
		
			AItemBase* SpawnedItem = GetWorld()->SpawnActor<AItemBase>(
				ItemClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

	}

	if (Box)
	{
		Box->PlayNiagaraEffect();
		for (TActorIterator<ADroneCharacter> It(GetWorld()); It; ++It)
		{
			It->RefreshTargetList();
		}
		// どのポイントから落とされたかを覚えさせる
		Box->LinkedDropPoint = CurrentTarget;

		MHP->AmmoWidget->UpdateDroneText(2);

		Destroy();
	}
}


//void ADroneCharacter::DropAmmoBox()
//{
//	if (!AmmoBoxClass) return;
//
//	FActorSpawnParameters Params;
//	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
//
//	// 足元にスポーン
//	FVector SpawnPos = GetActorLocation() - FVector(0, 0, 90);
//
//	AAmmoBox* Box = GetWorld()->SpawnActor<AAmmoBox>(AmmoBoxClass, SpawnPos, FRotator::ZeroRotator, Params);
//
//	if (Box)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Ammo Box Dropped!"));
//		Destroy();
//		
//	}
//}
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

	MHP->AmmoWidget->UpdateDroneText(0);

	Destroy();
}