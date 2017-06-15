// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_HitTask.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorph_HitTask : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
