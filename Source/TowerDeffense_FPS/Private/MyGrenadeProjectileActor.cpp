// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGrenadeProjectileActor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMyGrenadeProjectileActor::AMyGrenadeProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 見える用のメッシュ（任意）
	//UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//RootComponent = Mesh;

	LaunchSpeed = 1000.f; // デフォルト初速
	GravityScale = 1.f;   // 重力スケール

	// コリジョン
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("BlockAll");
	//CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyGrenadeProjectileActor::OnHit);
	RootComponent = CollisionComp;

	// 見た目
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMyGrenadeProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
	CalculateLaunchVelocity();
}

// Called every frame
void AMyGrenadeProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UEの重力はデフォルト -980 cm/s^2
	FVector Gravity(0.f, 0.f, -980.f * GravityScale);

	// 速度更新
	Velocity += Gravity * DeltaTime;

	// 位置更新
	SetActorLocation(GetActorLocation() + Velocity * DeltaTime);
}

void AMyGrenadeProjectileActor::CalculateLaunchVelocity()
{
    FVector Start = GetActorLocation();
    FVector Delta = TargetLocation - Start;
    float DeltaXY = FVector(Delta.X, Delta.Y, 0.f).Size();
    float DeltaZ = Delta.Z;
    float Gravity = 980.f * GravityScale;

    float SpeedSquared = LaunchSpeed * LaunchSpeed;
    float Discriminant = SpeedSquared * SpeedSquared - Gravity * (Gravity * DeltaXY * DeltaXY + 2 * DeltaZ * SpeedSquared);

    if (Discriminant < 0.f)
    {
        // 到達不可能 → 水平方向に飛ばすだけ
        Velocity = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal() * LaunchSpeed;
        return;
    }

    float Angle = FMath::Atan((SpeedSquared - FMath::Sqrt(Discriminant)) / (Gravity * DeltaXY));
    FVector DirXY = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();

    Velocity = DirXY * LaunchSpeed * FMath::Cos(Angle) + FVector(0.f, 0.f, LaunchSpeed * FMath::Sin(Angle));
}

void AMyGrenadeProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{


		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit, nullptr, this, nullptr);
		Destroy(); // 弾を消す
	}
}