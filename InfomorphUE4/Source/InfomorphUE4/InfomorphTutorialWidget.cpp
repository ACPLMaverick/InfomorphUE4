// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphTutorialWidget.h"
#include "InfomorphPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "InfomorphUE4.h"

void UInfomorphTutorialWidget::CloseTutorial()
{
	WidgetDisappear();
	PlayerController->SetTutorialWidget(nullptr);
	PlayerController = nullptr;
}

void UInfomorphTutorialWidget::Show(AInfomorphPlayerController* OwningPlayerController)
{
	if(OwningPlayerController == nullptr)
	{
		return;
	}

	PlayerController = OwningPlayerController;
	PlayerController->SetTutorialWidget(this);

	AddToViewport(999);

	WidgetAppear();
}

void UInfomorphTutorialWidget::WidgetDisappear_Implementation()
{
	RemoveFromViewport();
}