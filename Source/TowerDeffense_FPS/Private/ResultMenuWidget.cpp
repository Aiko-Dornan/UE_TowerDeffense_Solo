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
	SEPlay();
	// ステージセレクト用レベル名
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}

void UResultMenuWidget::OnTitleClicked()
{
	SEPlay();
	// ステージセレクト用レベル名
	UGameplayStatics::OpenLevel(this, FName("Title"));
}

void UResultMenuWidget::SEPlay()
{

	if (ClickSE)
	{
		UGameplayStatics::PlaySound2D(this, ClickSE);
	}

}