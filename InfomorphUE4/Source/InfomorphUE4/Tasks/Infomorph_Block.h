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

protected:
	UPROPERTY(EditAnywhere, Category = Node)
		float BlockTime;
	UPROPERTY(EditAnywhere, Category = Node)
		float RandomDeviation;
	UPROPERTY(EditAnywhere, Category = Node)
		bool bBlockForTime;

private:
	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};

struct FBlockTaskMemory
{
	float TimeRemaining;
};