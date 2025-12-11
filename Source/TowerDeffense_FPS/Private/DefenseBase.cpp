#include "DefenseBase.h"
#include "DefenseBaseHPWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include"TD_GameModeBase.h"

ADefenseBase::ADefenseBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // メッシュ作成
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    RootComponent = BaseMesh;

    MaxHealth = 500.0f;
    CurrentHealth = MaxHealth;

   
}

void ADefenseBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;

    ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM && GM->DefenseBaseHPWidget)
    {
        GM->DefenseBaseHPWidget->UpdateHP(CurrentHealth, MaxHealth);
    }

}

void ADefenseBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float ADefenseBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage <= 0.f) return 0.f;

    CurrentHealth -= ActualDamage;
    UE_LOG(LogTemp, Warning, TEXT("Base took %f damage! HP: %f/%f"), ActualDamage, CurrentHealth, MaxHealth);

    // HUD更新
    ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM && GM->DefenseBaseHPWidget)
    {
        GM->DefenseBaseHPWidget->UpdateHP(CurrentHealth, MaxHealth);
    }

    if (CurrentHealth <= 0.f)
    {
        OnDestroyedBase();
    }

    return ActualDamage;
}

void ADefenseBase::OnDestroyedBase()
{
    UE_LOG(LogTemp, Error, TEXT("The Defense Base has been destroyed!"));
    OnBaseDestroyed.Broadcast(); // Blueprintでゲームオーバーイベントを拾える

    // GameModeを取得してゲームオーバー実行
    ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
    {
        GM->GameOver();
    }

    // 破壊アニメやエフェクトなどを追加可能
    // ここでは単純に消す
    Destroy();
}
