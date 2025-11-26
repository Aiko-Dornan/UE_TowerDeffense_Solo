#include "AmmoBox.h"
#include "Components/BoxComponent.h"
#include "WeaponBase.h"
#include"MyHeroPlayer.h"
#include "GameFramework/Character.h"

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
}

void AAmmoBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if (!bPlayerInRange || !Weapon) return;

	
	
		GiveAmmoAmount = Weapon->MaxAmmo * 2;

		Weapon->StockAmmo += GiveAmmoAmount;
	
		IsAmmoGet = true;

		if (IsAmmoGet)
		{
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
