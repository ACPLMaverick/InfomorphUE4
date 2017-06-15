// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_Attack.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorph_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
