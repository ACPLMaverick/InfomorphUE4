// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Infomorph_RotateTo.generated.h"

struct FRotateToMemory
{
	float LerpTime;
};

/**
 * 
 */
UCLASS(config = Game)
class INFOMORPHUE4_API UInfomorph_RotateTo : public UBTTaskNode
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(config, Category = Node, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float RotateSpeed;

	UPROPERTY(config, Category = Node, EditAnywhere)
		FBlackboardKeySelector TargetKey;
	
private:
	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	EBTNodeResult::Type PerformRotate(class AInfomorphUE4Character* ControlledActor, class AInfomorphUE4Character* TargetActor, float LerpT);

public:
	virtual void OnGameplayTaskActivated(class UGameplayTask& Task);
};
