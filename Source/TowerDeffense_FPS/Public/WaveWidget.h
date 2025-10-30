#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WaveWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UWaveWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // ウェーブ数を表示するTextBlock
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WaveTextBlock;

    // ウェーブ数を更新する関数
    UFUNCTION(BlueprintCallable)
    void UpdateWaveText(int32 CurrentWave);
};

//#pragma once
//
//#include "CoreMinimal.h"
//#include "Blueprint/UserWidget.h"
//#include "WaveWidget.generated.h"
//
//UCLASS()
//class TOWERDEFFENSE_FPS_API UWaveWidget : public UUserWidget
//{
//    GENERATED_BODY()
//
//public:
//    UFUNCTION(BlueprintCallable, Category = "UI")
//    void UpdateWaveText(int32 CurrentWave, int32 MaxWave);
//
//protected:
//    virtual void NativeConstruct() override;
//
//    UPROPERTY()
//    class UTextBlock* WaveTextBlock;
//};
