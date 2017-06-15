// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_WaitTargetInterupt.generated.h"

struct FWaitTaskMemory
{
	float RemainingTime;
};

/**
 * 
 */
UCLASS(config = Game)
class INFOMORPHUE4_API UInfomorph_WaitTargetInterupt : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(config, Category = Node, EditAnywhere)
		float WaitTime;

	UPROPERTY(config, Category = Node, EditAnywhere)
		float RandomDeviation;

	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector TargetKey;

private:
	virtual FString GetStaticDescription() const override; 
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
	
};
