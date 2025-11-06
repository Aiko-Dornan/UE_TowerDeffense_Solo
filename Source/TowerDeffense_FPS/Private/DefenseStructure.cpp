#include "DefenseStructure.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "NavigationSystem.h"

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

    //  破壊エフェクト
    if (DestroyEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation(), FRotator::ZeroRotator, true);
    }

    //  サウンド再生
    if (DestroySound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation());
    }

    //  コリジョンとナビメッシュ影響を無効化
   /* MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MeshComp->SetCanEverAffectNavigation(false);*/

    FBox Bounds = MeshComp->Bounds.GetBox();

    const FVector Center = Bounds.GetCenter();
    const FVector Extent = Bounds.GetExtent() * 0.1f;  // ← 縮小率を調整（0.5-0.9推奨）

    

    //  ナビメッシュの再構築（範囲限定）
    if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        // 壊れたメッシュのバウンディングボックスをDirtyAreaとして登録
        //const FBox Bounds = MeshComp->Bounds.GetBox();
        Bounds = FBox(Center - Extent, Center + Extent);
        NavSys->AddDirtyArea(Bounds, ENavigationDirtyFlag::All);

        // NavMeshシステムがDynamic Runtime設定であれば、自動的に部分再生成される
    }

    //  少し遅らせて削除（NavSystemが更新完了するまで待機）
    SetLifeSpan(0.2f);

    //if (bIsDestroyed) return;
    //bIsDestroyed = true;

    //UE_LOG(LogTemp, Warning, TEXT("Defense Structure %s destroyed!"), *GetName());

    //// エフェクト再生
    //if (DestroyEffect)
    //{
    //    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyEffect, GetActorLocation(), FRotator::ZeroRotator, true);
    //}

    //// サウンド再生
    //if (DestroySound)
    //{
    //    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation());
    //}

    //// まずナビメッシュへの影響を無効化
    //MeshComp->SetCanEverAffectNavigation(false);
    //MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    //if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    //{
    //    FBox Bounds = MeshComp->Bounds.GetBox();
    //    NavSys->AddDirtyArea(Bounds, ENavigationDirtyFlag::All);
    //}

    //// 少し遅延して削除（フレーム更新を待つ）
    //SetLifeSpan(0.2f);

}
