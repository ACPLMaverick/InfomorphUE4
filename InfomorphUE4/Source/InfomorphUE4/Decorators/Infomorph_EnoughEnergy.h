// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Infomorph_EnoughEnergy.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorph_EnoughEnergy : public UBTDecorator
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Blackboard, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
		float MinimumEnergyLevel;

private:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	bool Evaluate(class AInfomorphUE4Character* OwnerCharacter) const;

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
};
