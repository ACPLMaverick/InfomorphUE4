// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Infomorph_ChaseTargetTask.generated.h"

/**
 * 
 */
UCLASS(config=Game)
class INFOMORPHUE4_API UInfomorph_ChaseTargetTask : public UBTTask_BlackboardBase
{
	GENERATED_UCLASS_BODY()

protected:
	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float AcceptableRadius;

	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float MaxDistanceFromInitLocation;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	EBTNodeResult::Type PerformMove(class AInfomorphBaseAIController* InfomorphAIController, AActor* Target);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
