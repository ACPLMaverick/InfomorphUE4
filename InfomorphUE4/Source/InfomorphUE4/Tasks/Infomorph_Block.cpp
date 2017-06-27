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
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

FString UInfomorph_Block::GetStaticDescription() const
{
	if(bBlockForTime)
	{
		return FString::Printf(TEXT("%s: Block for %.1f +- %.1f s"), *Super::GetStaticDescription(), BlockTime, RandomDeviation);
	}

	return Super::GetStaticDescription();
}

EBTNodeResult::Type UInfomorph_Block::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if(bBlockForTime)
	{
		FBlockTaskMemory* Memory = (FBlockTaskMemory*)NodeMemory;
		Memory->TimeRemaining = FMath::RandRange(-RandomDeviation, RandomDeviation) + BlockTime;
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

void UInfomorph_Block::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(bBlockForTime)
	{
		FBlockTaskMemory* Memory = (FBlockTaskMemory*)NodeMemory;
		Memory->TimeRemaining -= DeltaSeconds;

		if(Memory->TimeRemaining <= 0.0f)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}

uint16 UInfomorph_Block::GetInstanceMemorySize() const
{
	return sizeof(FBlockTaskMemory);
}

void UInfomorph_Block::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}