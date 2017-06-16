// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Infomorph_IsCloser.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorph_IsCloser : public UBTDecorator
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Blackboard, EditAnywhere)
		FBlackboardKeySelector SelfKey;
	UPROPERTY(Category = Blackboard, EditAnywhere)
		FBlackboardKeySelector TargetKey;
	UPROPERTY(Category = Blackboard, EditAnywhere)
		FBlackboardKeySelector TargetLocationKey;
	UPROPERTY(Category = Blackboard, EditAnywhere)
		bool bTargetCloserThanSelf;

private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	bool EvaluateOnBlackboard(const UBlackboardComponent& Blackboard) const;

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
};
