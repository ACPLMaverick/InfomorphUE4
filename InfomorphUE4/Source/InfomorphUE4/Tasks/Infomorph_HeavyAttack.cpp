// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_HeavyAttack.h"
#include "InfomorphBaseAIController.h"

UInfomorph_HeavyAttack::UInfomorph_HeavyAttack(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

EBTNodeResult::Type UInfomorph_HeavyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	bool Success = InfomorphAIController->PerformHeavyAttack();

	return Success ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UInfomorph_HeavyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	if(!InfomorphAIController->IsAttacking())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UInfomorph_HeavyAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}