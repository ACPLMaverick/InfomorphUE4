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
	bGenerateChaseOffset = true;
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

	if(bGenerateChaseOffset)
	{
		FVector ChaseOffset = FRotator(0.0f, FMath::FRandRange(-80.0f, 80.0f), 0.0f).RotateVector(TargetActor->GetActorForwardVector()) * AcceptableRadius * 0.8f;
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ChaseOffsetKey.SelectedKeyName, ChaseOffset);
	}

	EBTNodeResult::Type NodeResult = PerformMove(InfomorphAIController, TargetActor, OwnerComp.GetBlackboardComponent()->GetValueAsVector(ChaseOffsetKey.SelectedKeyName));

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
	EBTNodeResult::Type NodeResult = PerformMove(InfomorphAIController, TargetActor, OwnerComp.GetBlackboardComponent()->GetValueAsVector(ChaseOffsetKey.SelectedKeyName));
	
	if(NodeResult != EBTNodeResult::InProgress)
	{
		FinishLatentTask(OwnerComp, NodeResult);
	}
}

EBTNodeResult::Type UInfomorph_ChaseTarget::PerformMove(AInfomorphBaseAIController* InfomorphAIController, AActor* Target, const FVector& ChaseOffset)
{
	FVector TargetLocation = Target->GetActorLocation();

	AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	if(InfomorphCharacter == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	
	FVector InitLocation = InfomorphCharacter->GetInitialLocation();
	FVector SelfLocation = InfomorphCharacter->GetActorLocation();

	if(FVector::Dist(SelfLocation, TargetLocation) < AcceptableRadius)
	{
		return EBTNodeResult::Succeeded;
	}

	EPathFollowingRequestResult::Type Result = InfomorphAIController->MoveToLocation(TargetLocation, AcceptableRadius * 0.8f);

	return Result == EPathFollowingRequestResult::AlreadyAtGoal ? EBTNodeResult::Succeeded : EBTNodeResult::InProgress;
}
