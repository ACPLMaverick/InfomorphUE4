// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_GeneratePatrolLocation.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class INFOMORPHUE4_API UInfomorph_GeneratePatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float MinDistanceToPatrolLocation;

	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float MaxDistanceToPatrolLocation;

	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector PatrolLocationKey;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
