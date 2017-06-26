// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_Block.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "Engine/World.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UInfomorph_Block::UInfomorph_Block(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UInfomorph_Block::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	return InfomorphAIController->StartBlock() ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

void UInfomorph_Block::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController != nullptr)
	{
		InfomorphAIController->EndBlock();
	}
}

void UInfomorph_Block::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}