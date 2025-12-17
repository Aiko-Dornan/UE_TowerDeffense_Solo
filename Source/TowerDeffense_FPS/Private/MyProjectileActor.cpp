//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "MyProjectileActor.h"
//#include "GameFramework/ProjectileMovementComponent.h"
//#include "Components/SphereComponent.h"
//#include "Kismet/GameplayStatics.h"
//
//// Sets default values
//AMyProjectileActor::AMyProjectileActor()
//{
//    PrimaryActorTick.bCanEverTick = true;
//
//    // ============================
//    // Collision
//    // ============================
//    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
//    CollisionComp->InitSphereRadius(5.0f);
//
//    // ★重要：Projectile にすることで 弾同士は衝突しない
//    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
//
//    CollisionComp->OnComponentHit.AddDynamic(this, &AMyProjectileActor::OnHit);
//    RootComponent = CollisionComp;
//
//    // ============================
//    // Visual
//    // ============================
//    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
//    Mesh->SetupAttachment(RootComponent);
//    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//    // ============================
//    // Movement
//    // ============================
//    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
//    ProjectileMovement->InitialSpeed = 3000.f;
//    ProjectileMovement->MaxSpeed = 6000.f;
//    ProjectileMovement->bRotationFollowsVelocity = true;
//    ProjectileMovement->bShouldBounce = false;
//
//    // life
//    InitialLifeSpan = 3.0f;
//}
//
//// Called when the game starts or when spawned
//void AMyProjectileActor::BeginPlay()
//{
//    Super::BeginPlay();
//
//    // オーナー（撃った武器 / キャラ）との衝突は無視
//    if (AActor* OwnerActor = GetOwner())
//    {
//        CollisionComp->IgnoreActorWhenMoving(OwnerActor, true);
//    }
//}
//
//// Called every frame
//void AMyProjectileActor::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//}
//
//void AMyProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
//    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//    // 無効な衝突は無視
//    if (!OtherActor || OtherActor == this)
//        return;
//
//    // ダメージ適用
//    UGameplayStatics::ApplyPointDamage(
//        OtherActor,
//        Damage,
//        GetVelocity().GetSafeNormal(),
//        Hit,
//        nullptr,
//        this,
//        nullptr
//    );
//
//    // 弾を消す
//    Destroy();
//}


// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include "EnemyAIController.h"
#include "MyHeroPlayer.h"
#include "AllyCharacter.h"
#include "DefenseBase.h"
#include"DroneCharacter.h"
#include "DefenseStructure.h"

// Sets default values
AMyProjectileActor::AMyProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// コリジョン
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("BlockAll");
	//CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyProjectileActor::OnHit);
	RootComponent = CollisionComp;

	// 見た目
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// 移動
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// 寿命
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AMyProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	// スポーン直後は弾同士で当たらないように Ignore にする
	//CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//// Overlap を有効化
	//CollisionComp->SetGenerateOverlapEvents(true);
	//CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMyProjectileActor::OnOverlap);

	 // オーナー（撃った武器 / キャラ）との衝突は常に無視
	if (AActor* OwnerActor = GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(OwnerActor, true);
	}
	
	
	//CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 0.05秒後にコリジョンを有効化
	/*GetWorldTimerManager().SetTimer(TimerHandle_EnableCollision, this,
		&AMyProjectileActor::EnableCollision, 0.0001f, false);*/

}



// Called every frame
void AMyProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

void AMyProjectileActor::EnableCollision()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMyProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		
		if (OtherActor->IsA(AMyHeroPlayer::StaticClass()) ||
			OtherActor->IsA(AAllyCharacter::StaticClass()) ||
			OtherActor->IsA(ADroneCharacter::StaticClass()) ||
			/*Actor->IsA(ADefenseBase::StaticClass()) ||*/
			/*Actor->IsA(ADefenseStructure::StaticClass())||*/
			OtherActor->IsA(AEnemyCharacterBase::StaticClass()))
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit, nullptr, this, nullptr);
			PlayNiagaraEffect();
		}
		
		Destroy(); // 弾を消す
	}
}

void AMyProjectileActor::PlayNiagaraEffect()
{
	if (NiagaraEffect)
	{
		// ワールド上のこのアクタの位置にパーティクルをスポーン
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			NiagaraEffect,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(0.2f),  // スケール
			true,           // 自動破棄
			true,           // 自動アクティブ
			ENCPoolMethod::None,
			true
		);
	}
}