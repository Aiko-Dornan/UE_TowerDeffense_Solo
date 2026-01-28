#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ResultMenuWidget.generated.h"

UCLASS()
class TOWERDEFFENSE_FPS_API UResultMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ClearOrOverTextBlock;

protected:
	virtual void NativeConstruct() override;

	// ステージセレクトボタン
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_StageSelect;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Title;

	UFUNCTION()
	void OnStageSelectClicked();
	UFUNCTION()
	void OnTitleClicked();
};
