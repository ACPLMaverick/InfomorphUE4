// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_HitTask.h"
#include "InfomorphUE4Character.h"
#include "InfomorphBaseAIController.h"

UInfomorph_HitTask::UInfomorph_HitTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

EBTNodeResult::Type UInfomorph_HitTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	if(InfomorphCharacter == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	return InfomorphCharacter->WasHit() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

void UInfomorph_HitTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(OwnerComp.GetOwner());
	if(InfomorphCharacter == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FinishLatentTask(OwnerComp, InfomorphCharacter->WasHit() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
}

void UInfomorph_HitTask::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
