// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/UnrealEdEngine.h"
#include "MyGitEditor.generated.h"

/**
 * 
 */
UCLASS()
class GITEDITOR_API UMyGitEditor : public UUnrealEdEngine
{
	GENERATED_BODY()
public:
	void Init(IEngineLoop* InEngineLoop) override;
};
