// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_Chance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InfomorphBaseAIController.h"

bool UInfomorph_Chance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(OwnerComp.GetBlackboardComponent() == nullptr)
	{
		return false;
	}

	return EvaluateOnBlackboard(*(OwnerComp.GetBlackboardComponent()));
}

FString UInfomorph_Chance::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: random below %.2f"), *Super::GetStaticDescription(), Chance);
}

bool UInfomorph_Chance::EvaluateOnBlackboard(const UBlackboardComponent& Blackboard) const
{
	return FMath::RandRange(0.0f, 1.0f) <= Chance;
}

void UInfomorph_Chance::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UInfomorph_Chance::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
