// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "InfomorphTutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorphTutorialWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	class AInfomorphPlayerController* PlayerController;
	
public:
	void CloseTutorial();
	void Show(class AInfomorphPlayerController* OwningPlayerController);
};
