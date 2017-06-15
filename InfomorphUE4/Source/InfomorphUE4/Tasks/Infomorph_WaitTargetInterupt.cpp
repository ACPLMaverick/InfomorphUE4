// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_WaitTargetInterupt.h"
#include "BehaviorTree/BlackboardComponent.h"

UInfomorph_WaitTargetInterupt::UInfomorph_WaitTargetInterupt(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bNotifyTick = true;
}

FString UInfomorph_WaitTargetInterupt::GetStaticDescription() const
{
	if(FMath::IsNearlyZero(RandomDeviation))
	{
		return FString::Printf(TEXT("%s: %.1fs"), *Super::GetStaticDescription(), WaitTime);
	}
	else
	{
		return FString::Printf(TEXT("%s: %.1f+-%.1fs"), *Super::GetStaticDescription(), WaitTime, RandomDeviation);
	}
}

EBTNodeResult::Type UInfomorph_WaitTargetInterupt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FWaitTaskMemory* MyMemory = (FWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingTime = FMath::FRandRange(FMath::Max(0.0f, WaitTime - RandomDeviation), (WaitTime + RandomDeviation));

	return EBTNodeResult::InProgress;
}

uint16 UInfomorph_WaitTargetInterupt::GetInstanceMemorySize() const
{
	return sizeof(FWaitTaskMemory);
}

void UInfomorph_WaitTargetInterupt::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FWaitTaskMemory* MyMemory = (FWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingTime -= DeltaSeconds;

	if(MyMemory->RemainingTime <= 0.0f)
	{
		// continue execution from this node
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName) != nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

void UInfomorph_WaitTargetInterupt::OnGameplayTaskActivated(class UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}