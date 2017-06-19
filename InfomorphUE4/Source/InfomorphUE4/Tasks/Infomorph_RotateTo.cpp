// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_RotateTo.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UInfomorph_RotateTo::UInfomorph_RotateTo(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	AcceptableAngle = 5.0f;
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

	AInfomorphUE4Character* ControlledCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	AInfomorphUE4Character* TargetCharacter = Cast<AInfomorphUE4Character>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetCharacter != nullptr)
	{
		return PerformRotate(ControlledCharacter, TargetCharacter, 0.0f);
	}
	else
	{
		FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
		return PerformRotate(ControlledCharacter, TargetLocation, 0.0f);
	}
}

void UInfomorph_RotateTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AInfomorphUE4Character* ControlledCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	AInfomorphUE4Character* TargetCharacter = Cast<AInfomorphUE4Character>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetCharacter != nullptr)
	{
		EBTNodeResult::Type NodeResult = PerformRotate(ControlledCharacter, TargetCharacter, DeltaSeconds);
		FinishLatentTask(OwnerComp, NodeResult);
	}
	else
	{
		FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
		EBTNodeResult::Type NodeResult = PerformRotate(ControlledCharacter, TargetLocation, DeltaSeconds);
		FinishLatentTask(OwnerComp, NodeResult);
	}
}

EBTNodeResult::Type UInfomorph_RotateTo::PerformRotate(AInfomorphUE4Character* ControlledActor, AInfomorphUE4Character* TargetActor, float DeltaSeconds)
{
	if(ControlledActor == nullptr || TargetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	return PerformRotate(ControlledActor, TargetActor->GetActorLocation(), DeltaSeconds);
}

EBTNodeResult::Type UInfomorph_RotateTo::PerformRotate(AInfomorphUE4Character* ControlledActor, const FVector& TargetLocation, float DeltaSeconds)
{
	if(ControlledActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector Direction = TargetLocation - ControlledActor->GetActorLocation();
	Direction.Normalize();

	float TargetYaw = Direction.Rotation().Yaw;
	FRotator CurrentRotation = ControlledActor->GetActorRotation();
	if(FMath::Abs(TargetYaw - CurrentRotation.Yaw) > 180.0f)
	{
		TargetYaw = CurrentRotation.Yaw + FMath::Sign(CurrentRotation.Yaw) * (180.0f - FMath::Abs(CurrentRotation.Yaw) + 180.0f - FMath::Abs(TargetYaw));
	}

	CurrentRotation.Yaw = FMath::FInterpTo(CurrentRotation.Yaw, TargetYaw, DeltaSeconds, RotateSpeed);;
	ControlledActor->SetActorRotation(CurrentRotation);

	if(FMath::Abs(TargetYaw - CurrentRotation.Yaw) < AcceptableAngle)
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void UInfomorph_RotateTo::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
