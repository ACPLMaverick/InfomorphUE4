// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_StoreTargetLocation.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class INFOMORPHUE4_API UInfomorph_StoreTargetLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector TargetKey;
	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector TargetLocationKey;
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
