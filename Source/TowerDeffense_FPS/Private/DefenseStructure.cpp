#include "DefenseStructure.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"

ADefenseStructure::ADefenseStructure()
{
    PrimaryActorTick.bCanEverTick = false;

    // メッシュコンポーネントを作成
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // 攻撃を受けられるようにする
    MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    MeshComp->SetNotifyRigidBodyCollision(true);

    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
    MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ADefenseStructure::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;

    // Damage イベントを受け取れるようにする
    SetCanBeDamaged(true);

    UE_LOG(LogTemp, Warning, TEXT("%s can take damage: %d"), *GetName(), CanBeDamaged());

}

float ADefenseStructure::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDestroyed) return 0.f;

    UE_LOG(LogTemp, Warning, TEXT("TakeDamage called on %s!"), *GetName());

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("%s took %f damage! HP: %f/%f"),
        *GetName(), ActualDamage, CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        OnDestroyedByEnemy();
    }

    return ActualDamage;
}

void ADefenseStructure::OnDestroyedByEnemy()
{
    if (bIsDestroyed) return;
    bIsDestroyed = true;

    UE_LOG(LogTemp, Warning, TEXT("Defense Structure %s destroyed!"), *GetName());

    // エフェクト再生
    if (DestroyEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation(), FRotator::ZeroRotator, true);
    }

    // サウンド再生
    if (DestroySound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation());
    }

    // 一定時間後に削除
    Destroy();
}
