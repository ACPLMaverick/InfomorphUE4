// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_Distance.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UInfomorph_Distance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(OwnerComp.GetBlackboardComponent() == nullptr)
	{
		return false;
	}

	return EvaluateOnBlackboard(*(OwnerComp.GetBlackboardComponent()));
}

FString UInfomorph_Distance::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: distance between %s and %s <= (%.2f)"), *Super::GetStaticDescription(), *FirstKey.SelectedKeyName.ToString(), *SecondKey.SelectedKeyName.ToString(), MaxDistance);
}

bool UInfomorph_Distance::EvaluateOnBlackboard(const UBlackboardComponent& Blackboard) const
{
	FVector LocationA;
	FVector LocationB;

	AActor* First = Cast<AActor>(Blackboard.GetValueAsObject(FirstKey.SelectedKeyName));
	if(First == nullptr)
	{
		LocationA = Blackboard.GetValueAsVector(FirstKey.SelectedKeyName);
	}
	else
	{
		LocationA = First->GetActorLocation();
	}

	AActor* Second = Cast<AActor>(Blackboard.GetValueAsObject(SecondKey.SelectedKeyName));
	if(Second == nullptr)
	{
		LocationB = Blackboard.GetValueAsVector(SecondKey.SelectedKeyName);
	}
	else
	{
		LocationB = Second->GetActorLocation();
	}

	return FVector::Dist(LocationA, LocationB) <= MaxDistance;
}

void UInfomorph_Distance::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UInfomorph_Distance::OnGameplayTaskDeactivated(UGameplayTask & Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
