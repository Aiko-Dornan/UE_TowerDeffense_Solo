#include "ResultMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UResultMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_StageSelect)
	{
		Button_StageSelect->OnClicked.AddDynamic(
			this, &UResultMenuWidget::OnStageSelectClicked);
	}

	if (Button_Title)
	{
		Button_Title->OnClicked.AddDynamic(
			this, &UResultMenuWidget::OnTitleClicked);
	}

}

void UResultMenuWidget::OnStageSelectClicked()
{
	// ステージセレクト用レベル名
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}

void UResultMenuWidget::OnTitleClicked()
{
	// ステージセレクト用レベル名
	UGameplayStatics::OpenLevel(this, FName("Title"));
}
