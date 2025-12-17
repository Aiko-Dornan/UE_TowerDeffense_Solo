#include "AmmoBox.h"
#include "Components/BoxComponent.h"
#include "WeaponBase.h"
#include"MyHeroPlayer.h"
#include"DroneCharacter.h"
#include "DropPointActor.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"


//AAmmoBox::AAmmoBox()
//{
//	PrimaryActorTick.bCanEverTick = true;
//
//	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
//	RootComponent = CollisionBox;
//
//	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
//	CollisionBox->SetCollisionObjectType(ECC_WorldStatic);
//	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
//	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
//
//	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnOverlapBegin);
//	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &AAmmoBox::OnOverlapEnd);
//
//	// 見た目
//	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
//	Mesh->SetupAttachment(RootComponent);
//
//}

AAmmoBox::AAmmoBox()
{
	PrimaryActorTick.bCanEverTick = false;

	// 見た目のメッシュをルートにする
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	// 範囲用のBoxは子として追加
	RangeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RangeCollision"));
	RangeCollision->SetupAttachment(Mesh);

	RangeCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RangeCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RangeCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RangeCollision->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnRangeBegin);
	RangeCollision->OnComponentEndOverlap.AddDynamic(this, &AAmmoBox::OnRangeEnd);

	// Box のサイズを設定（例）
	RangeCollision->SetBoxExtent(FVector(256.f, 256.f, 380.f));
}

void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();

	UClass* MyClass = GetClass();
	AActor* OwnerActor = GetOwner();

	if (OwnerActor)
	{
		IsParentDrone = true;
	}
	/*else if (MyClass->GetOwnerClass())
	{
		IsParentDrone = true;
	}*/
	else if (MyClass->IsChildOf(APawn::StaticClass()))
	{
		IsParentDrone = true;
		UE_LOG(LogTemp, Warning, TEXT("This AmmoBox is a Pawn subclass"));
	}
	else if (MyClass->IsChildOf(ACharacter::StaticClass()))
	{
		IsParentDrone = true;
		UE_LOG(LogTemp, Warning, TEXT("This AmmoBox is a Character subclass"));
	}
	else
	{
		IsParentDrone = false;
		UE_LOG(LogTemp, Warning, TEXT("This AmmoBox has no Pawn/Character parent"));
	}

	//PlayNiagaraEffect();

	//// ★ Spawn した瞬間に範囲内にいるプレイヤーをチェックする
	//TArray<AActor*> OverlappingActors;
	//RangeCollision->GetOverlappingActors(OverlappingActors, AMyHeroPlayer::StaticClass());

	//for (AActor* Actor : OverlappingActors)
	//{
	//	if (AMyHeroPlayer* Player = Cast<AMyHeroPlayer>(Actor))
	//	{
	//		bPlayerInRange = true;
	//		OverlappingPlayer = Player;
	//		Player->ammobox = this;

	//		UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Player already inside AmmoBox range!"));
	//	}
	//}

}

void AAmmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PlayNiagaraEffect();
}

//void AAmmoBox::OnOverlapBegin(
//	UPrimitiveComponent* OverlappedComp,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex,
//	bool bFromSweep,
//	const FHitResult& SweepResult
//)
//{
//	if (OtherActor->IsA(ACharacter::StaticClass()))
//	{
//		bPlayerInRange = true;
//	}
//}
//
//void AAmmoBox::OnOverlapEnd(
//	UPrimitiveComponent* OverlappedComp,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex
//)
//{
//	if (OtherActor->IsA(ACharacter::StaticClass()))
//	{
//		bPlayerInRange = false;
//	}
//}

void AAmmoBox::TryGiveAmmo(AWeaponBase* Weapon)
{
	if (!bPlayerInRange || !Weapon||IsParentDrone) return;

	
	
		GiveAmmoAmount = Weapon->MaxAmmo * 2;

		Weapon->StockAmmo += GiveAmmoAmount;
	
		IsAmmoGet = true;

		if (IsAmmoGet)
		{
			if (LinkedDropPoint)
			{
				LinkedDropPoint->bAlreadyDropped = false;

				UE_LOG(LogTemp, Warning, TEXT("DropPoint %s is reset and available again!"),
					*LinkedDropPoint->GetName());

				for (TActorIterator<ADroneCharacter> It(GetWorld()); It; ++It)
				{
					It->RefreshTargetList();
				}

			}
			;
			NiagaraComp->Deactivate();
			Destroy();
		}
		

	// 必要なら現在のAmmoも瞬時にリロード
	// Weapon->StartReload();

	UE_LOG(LogTemp, Warning, TEXT("AmmoBox: Gave ammo! Stock: %f"), Weapon->StockAmmo);
}

void AAmmoBox::OnRangeBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMyHeroPlayer* Player = Cast<AMyHeroPlayer>(OtherActor))
	{
		bPlayerInRange = true;
		OverlappingPlayer = Player;

		// ★ プレイヤーへ「今の弾薬箱」をセット
		Player->ammobox = this;
	}
}

void AAmmoBox::OnRangeEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (AMyHeroPlayer* Player = Cast<AMyHeroPlayer>(OtherActor))
	{
		bPlayerInRange = false;
		OverlappingPlayer = nullptr;

		// ★ 出たのでプレイヤーの参照を消す
		Player->ammobox = nullptr;
	}
}

void AAmmoBox::PlayNiagaraEffect()
{
	if (NiagaraEffect)
	{
		// ワールド上のこのアクタの位置にパーティクルをスポーン
		NiagaraComp= UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NiagaraEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(4.0f),  // スケール
			false,           // 自動破棄
			true,           // 自動アクティブ
			ENCPoolMethod::None,
			true
		);

		UE_LOG(LogTemp, Warning, TEXT("AmmoBox: Niagara"));
	}
	
}

//void AAmmoBox::OnRangeBegin(UPrimitiveComponent* OverlappedComp,
//	AActor* OtherActor, UPrimitiveComponent* OtherComp,
//	int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (OtherActor->IsA(ACharacter::StaticClass()))
//	{
//		bPlayerInRange = true;
//	}
//}
//
//void AAmmoBox::OnRangeEnd(UPrimitiveComponent* OverlappedComp,
//	AActor* OtherActor, UPrimitiveComponent* OtherComp,
//	int32 BodyIndex)
//{
//	if (OtherActor->IsA(ACharacter::StaticClass()))
//	{
//		bPlayerInRange = false;
//	}
//}

//void AAmmoBox::TryGiveAmmo(AWeaponBase* Weapon)
//{
//	if (!Weapon) return;
//	GiveAmmoAmount = Weapon->MaxAmmo / 2;
//	Weapon->StockAmmo += GiveAmmoAmount;
//	//Weapon->OnAmmoChanged.Broadcast((int32)Weapon->Ammo, (int32)Weapon->StockAmmo);
//
//	UE_LOG(LogTemp, Warning, TEXT("AmmoBox: ammo supplied. Stock = %f"), Weapon->StockAmmo);
//}
