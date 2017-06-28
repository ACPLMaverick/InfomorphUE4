// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_ChaseTarget.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UInfomorph_ChaseTarget::UInfomorph_ChaseTarget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

void UInfomorph_ChaseTarget::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

EBTNodeResult::Type UInfomorph_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if(TargetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AInfomorphUE4Character* AICharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	if(AICharacter != nullptr)
	{
		AICharacter->SetMovementState(EMovementState::TargetLocked);
	}

	EBTNodeResult::Type NodeResult = PerformMove(InfomorphAIController, TargetActor);

	return NodeResult;
}

void UInfomorph_ChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if(TargetActor == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	EBTNodeResult::Type NodeResult = PerformMove(InfomorphAIController, TargetActor);
	
	if(NodeResult != EBTNodeResult::InProgress)
	{
		FinishLatentTask(OwnerComp, NodeResult);
	}
}

EBTNodeResult::Type UInfomorph_ChaseTarget::PerformMove(AInfomorphBaseAIController* InfomorphAIController, AActor* Target)
{
	AInfomorphUE4Character* OwnerCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController);
	if(OwnerCharacter != nullptr && OwnerCharacter->IsConfused())
	{
		InfomorphAIController->StopMovement();
		return EBTNodeResult::Failed;
	}

	EPathFollowingRequestResult::Type MoveResult =  InfomorphAIController->MoveToActor(Target, AcceptableRadius, false, true, false);

	return MoveResult == EPathFollowingRequestResult::AlreadyAtGoal ? EBTNodeResult::Succeeded : EBTNodeResult::InProgress;
}
