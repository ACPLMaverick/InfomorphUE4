// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_RotateTo.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UInfomorph_RotateTo::UInfomorph_RotateTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

FString UInfomorph_RotateTo::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *(TargetKey.SelectedKeyName.ToString()));
}

EBTNodeResult::Type UInfomorph_RotateTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FRotateToMemory* Memory = reinterpret_cast<FRotateToMemory*>(NodeMemory);
	Memory->LerpTime = 0.0f;

	AInfomorphUE4Character* ControlledCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	AInfomorphUE4Character* TargetCharacter = Cast<AInfomorphUE4Character>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetCharacter != nullptr)
	{
		return PerformRotate(ControlledCharacter, TargetCharacter, Memory->LerpTime);
	}
	else
	{
		FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
		return PerformRotate(ControlledCharacter, TargetLocation, Memory->LerpTime);
	}
}

uint16 UInfomorph_RotateTo::GetInstanceMemorySize() const
{
	return sizeof(FRotateToMemory);
}

void UInfomorph_RotateTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FRotateToMemory* Memory = reinterpret_cast<FRotateToMemory*>(NodeMemory);
	Memory->LerpTime += DeltaSeconds * RotateSpeed;

	AInfomorphUE4Character* ControlledCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	AInfomorphUE4Character* TargetCharacter = Cast<AInfomorphUE4Character>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetCharacter != nullptr)
	{
		EBTNodeResult::Type NodeResult = PerformRotate(ControlledCharacter, TargetCharacter, Memory->LerpTime);
		FinishLatentTask(OwnerComp, NodeResult);
	}
	else
	{
		FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
		EBTNodeResult::Type NodeResult = PerformRotate(ControlledCharacter, TargetLocation, Memory->LerpTime);
		FinishLatentTask(OwnerComp, NodeResult);
	}
}

EBTNodeResult::Type UInfomorph_RotateTo::PerformRotate(AInfomorphUE4Character* ControlledActor, AInfomorphUE4Character* TargetActor, float LerpT)
{
	if(ControlledActor == nullptr || TargetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	return PerformRotate(ControlledActor, TargetActor->GetActorLocation(), LerpT);
}

EBTNodeResult::Type UInfomorph_RotateTo::PerformRotate(AInfomorphUE4Character* ControlledActor, const FVector& TargetLocation, float LerpT)
{
	if(LerpT > 1.0f)
	{
		return EBTNodeResult::Succeeded;
	}

	FVector Direction = TargetLocation - ControlledActor->GetActorLocation();

	FRotator CurrentRotation = ControlledActor->GetActorRotation();
	CurrentRotation.Yaw = ControlledActor->CalculateTargetYaw(CurrentRotation, Direction.Rotation(), FMath::Clamp(LerpT, 0.0f, 1.0f));
	ControlledActor->SetActorRotation(CurrentRotation);

	return EBTNodeResult::InProgress;
}

void UInfomorph_RotateTo::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
