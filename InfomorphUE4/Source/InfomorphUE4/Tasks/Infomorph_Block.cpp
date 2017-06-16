// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_Block.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4.h"
#include "Engine/World.h"

UInfomorph_Block::UInfomorph_Block(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UInfomorph_Block::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FBlockMemory* MyMemory = (FBlockMemory*)NodeMemory;
	float CurrentTime = InfomorphAIController->GetWorld()->GetRealTimeSeconds();
	if(MyMemory->LastBlockTime > 0.0f && CurrentTime - MyMemory->LastBlockTime < Cooldown + MyMemory->RemainingTime)
	{
		return EBTNodeResult::Failed;
	}

	MyMemory->LastBlockTime = CurrentTime;
	MyMemory->RemainingTime = FMath::FRandRange(FMath::Max(0.0f, BlockTime - RandomDeviation), (BlockTime + RandomDeviation));

	return InfomorphAIController->StartBlock() ? EBTNodeResult::InProgress : EBTNodeResult::Failed;
}

FString UInfomorph_Block::GetStaticDescription() const
{
	if(FMath::IsNearlyZero(RandomDeviation))
	{
		return FString::Printf(TEXT("%s: Block for %.1fs"), *Super::GetStaticDescription(), BlockTime);
	}
	else
	{
		return FString::Printf(TEXT("%s: Block for %.1f+-%.1fs"), *Super::GetStaticDescription(), BlockTime, RandomDeviation);
	}
}

void UInfomorph_Block::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBlockMemory* MyMemory = (FBlockMemory*)NodeMemory;
	MyMemory->RemainingTime -= DeltaSeconds;

	if(MyMemory->RemainingTime <= 0.0f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UInfomorph_Block::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController != nullptr)
	{
		InfomorphAIController->EndBlock();
	}
}

uint16 UInfomorph_Block::GetInstanceMemorySize() const
{
	return sizeof(FBlockMemory);
}

void UInfomorph_Block::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}