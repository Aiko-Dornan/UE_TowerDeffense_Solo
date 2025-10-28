#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"   // �� ����厖�IAActor���g������
#include "Delegates/DelegateCombinations.h"
#include "DefenseBase.generated.h" // �� �K���Ō�ɏ���

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBaseDestroyed);

UCLASS()
class TOWERDEFFENSE_FPS_API ADefenseBase : public AActor   // �� UObject�ł͂Ȃ�AActor�I
{
    GENERATED_BODY()

public:
    ADefenseBase();

    virtual void Tick(float DeltaTime) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;  // �� AActor�������Ă���֐��iUObject�ɂ͂Ȃ��j

    void OnDestroyedBase();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
    float MaxHealth = 500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Stats")
    float CurrentHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base Mesh")
    UStaticMeshComponent* BaseMesh;

    

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnBaseDestroyed OnBaseDestroyed;
};
