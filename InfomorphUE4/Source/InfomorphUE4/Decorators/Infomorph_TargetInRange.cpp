// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_TargetInRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InfomorphBaseAIController.h"

bool UInfomorph_TargetInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(OwnerComp.GetBlackboardComponent() == nullptr)
	{
		return false;
	}

	return EvaluateOnBlackboard(*(OwnerComp.GetBlackboardComponent()));
}

FString UInfomorph_TargetInRange::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: distance between SELF and %s <= (%.2f)"), *Super::GetStaticDescription(), *TargetKey.SelectedKeyName.ToString(), Range);
}

bool UInfomorph_TargetInRange::EvaluateOnBlackboard(const UBlackboardComponent& Blackboard) const
{
	AActor* TargetActor = Cast<AActor>(Blackboard.GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetActor == nullptr)
	{
		return false;
	}

	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(Blackboard.GetBrainComponent()->GetAIOwner());
	if(InfomorphAIController == nullptr || InfomorphAIController->GetPawn() == nullptr)
	{
		return false;
	}

	return FVector::Dist(TargetActor->GetActorLocation(), InfomorphAIController->GetPawn()->GetActorLocation()) <= Range;
}

void UInfomorph_TargetInRange::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UInfomorph_TargetInRange::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
