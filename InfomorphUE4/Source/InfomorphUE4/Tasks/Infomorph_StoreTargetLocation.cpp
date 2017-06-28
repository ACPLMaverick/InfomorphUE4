// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_StoreTargetLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UInfomorph_StoreTargetLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector TargetLocation = Target->GetActorLocation();
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(TargetLocationKey.SelectedKeyName, TargetLocation);

	return EBTNodeResult::Succeeded;
}

void UInfomorph_StoreTargetLocation::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
