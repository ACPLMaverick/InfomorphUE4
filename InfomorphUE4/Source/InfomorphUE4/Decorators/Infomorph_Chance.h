// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Infomorph_Chance.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorph_Chance : public UBTDecorator
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Blackboard, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
		float Chance;

private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	bool EvaluateOnBlackboard(const UBlackboardComponent& Blackboard) const;

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
};
