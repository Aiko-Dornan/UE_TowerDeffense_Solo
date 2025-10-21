// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyProjectileActor::AMyProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ÉRÉäÉWÉáÉì
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("BlockAll");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyProjectileActor::OnHit);
	RootComponent = CollisionComp;

	// å©ÇΩñ⁄
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// à⁄ìÆ
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// éıñΩ
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AMyProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit, nullptr, this, nullptr);
		Destroy(); // íeÇè¡Ç∑
	}
}

