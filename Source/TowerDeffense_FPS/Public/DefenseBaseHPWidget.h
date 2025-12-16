#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "DefenseBaseHPWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UDefenseBaseHPWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HPBar;
   
    /*float CHP = 500.f;
    float MHP = 500.f;*/

    //void NativeConstruct();
    UFUNCTION(BlueprintCallable)
    void UpdateHP(float CurrentHP, float MaxHP);
   
};