#include "MyGrenadeProjectileActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "MyHeroPlayer.h"
#include "AllyCharacter.h"
#include "DefenseBase.h"
#include"DroneCharacter.h"
#include "DefenseStructure.h"
#include"AIController.h"


// Sets default values
AMyGrenadeProjectileActor::AMyGrenadeProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	LaunchSpeed = 1000.f;
	GravityScale = 1.f;
	Damage = 20.f;

	// コリジョン
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("BlockAll");
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyGrenadeProjectileActor::OnHit);
	RootComponent = CollisionComp;

	// メッシュ
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AMyGrenadeProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(OwnerActor, true);
	}

	CalculateLaunchVelocity();

	// 弾の向きを初期速度に合わせる
	Mesh->SetWorldRotation(Velocity.Rotation());
}

// Called every frame
void AMyGrenadeProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Gravity(0.f, 0.f, -980.f * GravityScale);

	// 速度更新
	Velocity += Gravity * DeltaTime;

	// 位置更新
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime,true);

	// 弾の見た目を飛行方向に向ける
	Mesh->SetWorldRotation(Velocity.Rotation());
}

void AMyGrenadeProjectileActor::CalculateLaunchVelocity()
{
	FVector Start = GetActorLocation();
	FVector Delta = TargetLocation - Start;

	// 水平方向距離
	float DeltaXY = FVector(Delta.X, Delta.Y, 0.f).Size();
	float DeltaZ = Delta.Z;
	float Gravity = 980.f * GravityScale;

	float SpeedSquared = LaunchSpeed * LaunchSpeed;
	float Discriminant = SpeedSquared * SpeedSquared - Gravity * (Gravity * DeltaXY * DeltaXY + 2 * DeltaZ * SpeedSquared);

	FVector DirXY = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();

	if (Discriminant < 0.f)
	{
		// 初速不足の場合でもターゲット方向に向ける
		Velocity = Delta.GetSafeNormal() * LaunchSpeed;
		UE_LOG(LogTemp, Warning, TEXT("Target unreachable with current LaunchSpeed, firing towards target anyway."));
		return;
	}

	// 放物線の角度を計算
	float Angle = FMath::Atan((SpeedSquared - FMath::Sqrt(Discriminant)) / (Gravity * DeltaXY));

	Velocity = DirXY * LaunchSpeed * FMath::Cos(Angle) + FVector(0.f, 0.f, LaunchSpeed * FMath::Sin(Angle));
}

void AMyGrenadeProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s (comp: %s)"),
		*OtherActor->GetName(),
		*OtherComp->GetName());

	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit!!"));

		if (ExplosionFlag)
		{
			
			UE_LOG(LogTemp, Warning, TEXT("explosion!!"));
			ApplyAreaDamage(Damage, AmountAreaAtacck);
		}
		else
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, Damage, Velocity.GetSafeNormal(), Hit, nullptr, this, UDamageType::StaticClass());
		}

		
		Destroy();
	}
}

// ==================== 範囲攻撃（全対象に即時ダメージ） ====================
void AMyGrenadeProjectileActor::ApplyAreaDamage(float DamageAmount, float Radius)
{
	FHitResult Hit;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector Center = GetActorLocation();   // プレイヤーの位置
	float Radius2 = Radius;           // 検索半径（例：500.f）

	// 取得結果を入れる配列
	TArray<AActor*> FoundActors;

	// 取得対象の ObjectType（例：WorldDynamic）
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	// 無視する Actor（自分）
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		Center,
		Radius,
		ObjectTypes,
		AActor::StaticClass(),  // 取得したいクラス
		IgnoreActors,
		FoundActors
	);

	if (bHit)
	{
		for (AActor* Actor : FoundActors)
		{
			if (PlayerFlag)
			{
				// --- 範囲ダメージを与える対象 ---
				if (/*Actor->IsA(AMyHeroPlayer::StaticClass()) ||*/
					Actor->IsA(AAllyCharacter::StaticClass()) ||
					Actor->IsA(ADroneCharacter::StaticClass()) ||
					/*Actor->IsA(ADefenseBase::StaticClass()) ||*/
					/*Actor->IsA(ADefenseStructure::StaticClass())||*/
					Actor->IsA(AEnemyCharacterBase::StaticClass()))
				{
					/*UGameplayStatics::ApplyDamage(
						Actor,
						DamageAmount,
						GetController(),
						this,
						nullptr
					);*/

					UGameplayStatics::ApplyPointDamage(Actor, Damage, Velocity.GetSafeNormal(), Hit, nullptr, this, UDamageType::StaticClass());

					UE_LOG(LogTemp, Warning, TEXT("%s - AreaDamage applied to %s巻き込みました!!"),
						*GetName(), *Actor->GetName());
				}
			}
			else
			{
				// --- 範囲ダメージを与える対象 ---
				if (Actor->IsA(AMyHeroPlayer::StaticClass()) ||
					Actor->IsA(AAllyCharacter::StaticClass()) ||
					Actor->IsA(ADroneCharacter::StaticClass()) ||
					Actor->IsA(ADefenseBase::StaticClass()) ||
					Actor->IsA(ADefenseStructure::StaticClass()))
				{
					

					UGameplayStatics::ApplyPointDamage(Actor, Damage, Velocity.GetSafeNormal(), Hit, nullptr, this, UDamageType::StaticClass());

					UE_LOG(LogTemp, Warning, TEXT("%s - AreaDamage applied to %s巻き込みました!!"),
						*GetName(), *Actor->GetName());
				}
			}

			

			UE_LOG(LogTemp, Warning, TEXT("Actor in range: %s"), *Actor->GetName());
		}
	}


	PlayNiagaraEffect();
	
}


void AMyGrenadeProjectileActor::PlayNiagaraEffect()
{
	if (NiagaraEffect)
	{
		// ワールド上のこのアクタの位置にパーティクルをスポーン
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NiagaraEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.0f),  // スケール
			true,           // 自動破棄
			true,           // 自動アクティブ
			ENCPoolMethod::None,
			true
		);
	}
}
 
// 
//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "MyGrenadeProjectileActor.h"
//#include "Components/StaticMeshComponent.h"
//#include "Kismet/KismetMathLibrary.h"
//#include "Components/SphereComponent.h"
//#include "Kismet/GameplayStatics.h"
//
//
//// Sets default values
//AMyGrenadeProjectileActor::AMyGrenadeProjectileActor()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	// 見える用のメッシュ（任意）
//	//UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
//	//RootComponent = Mesh;
//
//	LaunchSpeed = 1000.f; // デフォルト初速
//	GravityScale = 1.f;   // 重力スケール
//
//	// コリジョン
//	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
//	CollisionComp->InitSphereRadius(5.0f);
//	CollisionComp->SetCollisionProfileName("BlockAll");
//	//CollisionComp->SetCollisionProfileName("Projectile");
//	CollisionComp->OnComponentHit.AddDynamic(this, &AMyGrenadeProjectileActor::OnHit);
//	RootComponent = CollisionComp;
//
//	// 見た目
//	
//	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
//	Mesh->SetupAttachment(RootComponent);
//
//}
//
//// Called when the game starts or when spawned
//void AMyGrenadeProjectileActor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//	CalculateLaunchVelocity();
//}
//
//// Called every frame
//void AMyGrenadeProjectileActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	// UEの重力はデフォルト -980 cm/s^2
//	FVector Gravity(0.f, 0.f, -980.f * GravityScale);
//
//	// 速度更新
//	Velocity += Gravity * DeltaTime;
//
//	// 位置更新
//	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
//}
//
//void AMyGrenadeProjectileActor::CalculateLaunchVelocity()
//{
//    FVector Start = GetActorLocation();
//    FVector Delta = TargetLocation - Start;
//    float DeltaXY = FVector(Delta.X, Delta.Y, 0.f).Size();
//    float DeltaZ = Delta.Z;
//    float Gravity = 980.f * GravityScale;
//
//    float SpeedSquared = LaunchSpeed * LaunchSpeed;
//    float Discriminant = SpeedSquared * SpeedSquared - Gravity * (Gravity * DeltaXY * DeltaXY + 2 * DeltaZ * SpeedSquared);
//
//    if (Discriminant < 0.f)
//    {
//        // 到達不可能 → 水平方向に飛ばすだけ
//        Velocity = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal() * LaunchSpeed;
//        return;
//    }
//
//    float Angle = FMath::Atan((SpeedSquared - FMath::Sqrt(Discriminant)) / (Gravity * DeltaXY));
//    FVector DirXY = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();
//
//    Velocity = DirXY * LaunchSpeed * FMath::Cos(Angle) + FVector(0.f, 0.f, LaunchSpeed * FMath::Sin(Angle));
//}
//
//void AMyGrenadeProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	if (OtherActor && OtherActor != this)
//	{
//
//
//		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit, nullptr, this, nullptr);
//		Destroy(); // 弾を消す
//	}
//}