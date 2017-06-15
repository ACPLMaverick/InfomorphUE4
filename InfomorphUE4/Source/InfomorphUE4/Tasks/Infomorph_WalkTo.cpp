// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_WalkTo.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"

#include "BehaviorTree/BlackboardComponent.h"

UInfomorph_WalkTo::UInfomorph_WalkTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

void UInfomorph_WalkTo::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UInfomorph_WalkTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if(OwnerComp.GetBlackboardComponent() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetActor != nullptr)
	{
		return EBTNodeResult::Failed;
	}

	EBTNodeResult::Type NodeResult = PerformMove(InfomorphAIController, OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName));

	return NodeResult;
}

void UInfomorph_WalkTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if(OwnerComp.GetBlackboardComponent() == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetActor != nullptr)
	{
		InfomorphAIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	EBTNodeResult::Type NodeResult = PerformMove(InfomorphAIController, OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName));

	if(NodeResult != EBTNodeResult::InProgress)
	{
		InfomorphAIController->StopMovement();
		FinishLatentTask(OwnerComp, NodeResult);
	}
}

EBTNodeResult::Type UInfomorph_WalkTo::PerformMove(AInfomorphBaseAIController* InfomorphAIController, const FVector& TargetLocation)
{
	EPathFollowingRequestResult::Type Result = InfomorphAIController->MoveToLocation(TargetLocation, AcceptableRadius);

	return Result == EPathFollowingRequestResult::AlreadyAtGoal ? EBTNodeResult::Succeeded : EBTNodeResult::InProgress;
}
