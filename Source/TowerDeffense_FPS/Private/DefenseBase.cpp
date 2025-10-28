#include "DefenseBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include"TD_GameModeBase.h"

ADefenseBase::ADefenseBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // ���b�V���쐬
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    RootComponent = BaseMesh;

    MaxHealth = 500.0f;
    CurrentHealth = MaxHealth;
}

void ADefenseBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
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

    if (CurrentHealth <= 0.f)
    {
        OnDestroyedBase();
    }

    return ActualDamage;
}

void ADefenseBase::OnDestroyedBase()
{
    UE_LOG(LogTemp, Error, TEXT("The Defense Base has been destroyed!"));
    OnBaseDestroyed.Broadcast(); // Blueprint�ŃQ�[���I�[�o�[�C�x���g���E����

    // GameMode���擾���ăQ�[���I�[�o�[���s
    ATD_GameModeBase* GM = Cast<ATD_GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
    {
        GM->GameOver();
    }

    // �j��A�j����G�t�F�N�g�Ȃǂ�ǉ��\
    // �����ł͒P���ɏ���
    Destroy();
}
