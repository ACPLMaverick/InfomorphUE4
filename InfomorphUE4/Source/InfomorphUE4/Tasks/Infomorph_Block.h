// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_Block.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class INFOMORPHUE4_API UInfomorph_Block : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
