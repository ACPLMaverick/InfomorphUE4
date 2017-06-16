// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_WalkTo.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UInfomorph_WalkTo::UInfomorph_WalkTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	PatrolSpeed = 200.0f;
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

	ACharacter* PossessedCharacter = Cast<ACharacter>(InfomorphAIController->GetPawn());
	if(PossessedCharacter != nullptr)
	{
		FWalkToMemory* Memory = reinterpret_cast<FWalkToMemory*>(NodeMemory);
		Memory->PreviousMaxWalkingSpeed = PossessedCharacter->GetCharacterMovement()->MaxWalkSpeed;
		PossessedCharacter->GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
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

void UInfomorph_WalkTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return;
	}

	ACharacter* PossessedCharacter = Cast<ACharacter>(InfomorphAIController->GetPawn());
	if(PossessedCharacter != nullptr)
	{
		FWalkToMemory* Memory = reinterpret_cast<FWalkToMemory*>(NodeMemory);
		PossessedCharacter->GetCharacterMovement()->MaxWalkSpeed = Memory->PreviousMaxWalkingSpeed;
	}
}

uint16 UInfomorph_WalkTo::GetInstanceMemorySize() const
{
	return sizeof(FWalkToMemory);
}

EBTNodeResult::Type UInfomorph_WalkTo::PerformMove(AInfomorphBaseAIController* InfomorphAIController, const FVector& TargetLocation)
{
	EPathFollowingRequestResult::Type Result = InfomorphAIController->MoveToLocation(TargetLocation, AcceptableRadius);

	return Result == EPathFollowingRequestResult::AlreadyAtGoal ? EBTNodeResult::Succeeded : EBTNodeResult::InProgress;
}
