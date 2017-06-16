// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_WalkTo.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class INFOMORPHUE4_API UInfomorph_WalkTo : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(config, Category = Node, EditAnywhere)
		float AcceptableRadius;
	
	UPROPERTY(config, Category = Node, EditAnywhere)
		float PatrolSpeed;

	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector TargetKey;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	EBTNodeResult::Type PerformMove(class AInfomorphBaseAIController* InfomorphAIController, const FVector& TargetLocation);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
	virtual uint16 GetInstanceMemorySize() const override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};

struct FWalkToMemory
{
	float PreviousMaxWalkingSpeed;
};