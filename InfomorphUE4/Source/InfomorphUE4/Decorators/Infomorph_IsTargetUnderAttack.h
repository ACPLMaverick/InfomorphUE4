// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Infomorph_IsTargetUnderAttack.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorph_IsTargetUnderAttack : public UBTDecorator
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Blackboard, EditAnywhere)
		FBlackboardKeySelector TargetKey;

private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	bool EvaluateOnBlackboard(const UBlackboardComponent& Blackboard, APawn* ControlledPawn) const;

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
};
