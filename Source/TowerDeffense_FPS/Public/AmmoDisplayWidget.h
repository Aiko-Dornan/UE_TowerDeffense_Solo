// AmmoDisplay.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "AmmoDisplayWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UAmmoDisplay : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmmoTextBlock;

   /* UPROPERTY(meta = (BindWidget))
    UTextBlock* ReloadTextBlock;*/

    // �e���X�V�֐�
    UFUNCTION(BlueprintCallable)
    void UpdateAmmoText(int32 CurrentAmmo, int32 StockAmmo);

    // �����[�h��Ԑؑ�
    UFUNCTION(BlueprintCallable)
    void SetReloading(bool bIsReloading);
};