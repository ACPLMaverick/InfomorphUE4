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
	
	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	if(Character != nullptr)
	{
		MyMemory->PreviousMaxSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
	}

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

	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	if(Character != nullptr && Target != nullptr)
	{
		if(FVector::Dist(Character->GetActorLocation(), Target->GetActorLocation()) > TargetDistanceFromTarget)
		{
			InfomorphAIController->MoveToLocation(Target->GetActorLocation(), TargetDistanceFromTarget);
		}
	}
}

void UInfomorph_Block::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController != nullptr)
	{
		InfomorphAIController->EndBlock();

		AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
		if(Character != nullptr)
		{
			FBlockMemory* MyMemory = (FBlockMemory*)NodeMemory;
			Character->GetCharacterMovement()->MaxWalkSpeed = MyMemory->PreviousMaxSpeed;
		}
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