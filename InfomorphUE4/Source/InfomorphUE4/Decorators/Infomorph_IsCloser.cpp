// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_IsCloser.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UInfomorph_IsCloser::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	return BlackboardComponent && EvaluateOnBlackboard(*BlackboardComponent);
}

FString UInfomorph_IsCloser::GetStaticDescription() const
{
	if(bTargetCloserThanSelf)
	{
		return FString::Printf(TEXT("%s: %s is closer to %s than %s"), *Super::GetStaticDescription(), *TargetKey.SelectedKeyName.ToString(), *TargetLocationKey.SelectedKeyName.ToString(), *SelfKey.SelectedKeyName.ToString());
	}
	else
	{
		return FString::Printf(TEXT("%s: %s is closer to %s than %s"), *Super::GetStaticDescription(), *SelfKey.SelectedKeyName.ToString(), *TargetLocationKey.SelectedKeyName.ToString(), *TargetKey.SelectedKeyName.ToString());
	}
}

bool UInfomorph_IsCloser::EvaluateOnBlackboard(const UBlackboardComponent& Blackboard) const
{
	AActor* Self = Cast<AActor>(Blackboard.GetValueAsObject(SelfKey.SelectedKeyName));
	AActor* Target = Cast<AActor>(Blackboard.GetValueAsObject(TargetKey.SelectedKeyName));
	FVector TargetLocation = Blackboard.GetValueAsVector(TargetLocationKey.SelectedKeyName);
	
	if(Self == nullptr || Target == nullptr)
	{
		return false;
	}

	bool bCloser = FVector::Dist(Self->GetActorLocation(), TargetLocation) < FVector::Dist(Target->GetActorLocation(), TargetLocation);
	if(bTargetCloserThanSelf)
	{
		bCloser = FVector::Dist(Self->GetActorLocation(), TargetLocation) > FVector::Dist(Target->GetActorLocation(), TargetLocation);
	}
	return bCloser;
}

void UInfomorph_IsCloser::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UInfomorph_IsCloser::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
