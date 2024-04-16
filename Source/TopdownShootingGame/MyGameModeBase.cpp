// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "Blueprint/UserWidget.h"


void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidjet(StartingWidgetClass);
}

void AMyGameModeBase::ChangeMenuWidjet(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
