// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGitEditor.h"

#include "ISourceControlModule.h"

void UMyGitEditor::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);

	const ISourceControlModule& SourceControlModule = ISourceControlModule::Get();
	{
		ISourceControlProvider& SourceControlProvider = SourceControlModule.GetProvider();

		const TArray<FString> Branches{ "origin/main", "origin/develop" };

		SourceControlProvider.RegisterStateBranches(Branches, TEXT("Content"));
	}
}