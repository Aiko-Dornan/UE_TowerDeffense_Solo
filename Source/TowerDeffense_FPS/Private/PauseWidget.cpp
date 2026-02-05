#include "PauseWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.h"
#include"MyHeroPlayer.h"

void UPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseWidget::OnClickedResume);
	}

	if (SSBackButton)
	{
		SSBackButton->OnClicked.AddDynamic(this, &UPauseWidget::OnClickStageSelectBack);
	}

}

void UPauseWidget::OnClickedResume()
{
	UE_LOG(LogTemp, Warning, TEXT("end2"));
	if (APlayerController* PC = GetOwningPlayer())
	{
		UE_LOG(LogTemp, Warning, TEXT("end1"));
		if (AMyHeroPlayer* MyPC = Cast<AMyHeroPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyHeroPlayer::StaticClass())))
		{
			MyPC->ResumeGame();
			UE_LOG(LogTemp, Warning, TEXT("end"));
		}
	}
}

void UPauseWidget::OnClickStageSelectBack()
{
	UGameplayStatics::OpenLevel(this, FName("StageSelect"));
}