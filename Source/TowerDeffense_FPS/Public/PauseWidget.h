// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"MyHeroPlayer.h"
#include "PauseWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class TOWERDEFFENSE_FPS_API UPauseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* SSBackButton;

	UFUNCTION()
	void OnClickedResume();
	UFUNCTION()
	void OnClickStageSelectBack();
public:
	
};
