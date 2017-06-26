// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_IsTargetUnderAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"

bool UInfomorph_IsTargetUnderAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(OwnerComp.GetBlackboardComponent() == nullptr)
	{
		return false;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if(Controller == nullptr)
	{
		return false;
	}

	return EvaluateOnBlackboard(*(OwnerComp.GetBlackboardComponent()), Controller->GetPawn());
}

FString UInfomorph_IsTargetUnderAttack::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Is %s under attack"), *Super::GetStaticDescription(), *TargetKey.SelectedKeyName.ToString());
}

bool UInfomorph_IsTargetUnderAttack::EvaluateOnBlackboard(const UBlackboardComponent& Blackboard, APawn* ControlledPawn) const
{
	if(ControlledPawn == nullptr)
	{
		return false;
	}

	AInfomorphUE4Character* TargetActor = Cast<AInfomorphUE4Character>(Blackboard.GetValueAsObject(TargetKey.SelectedKeyName));
	if(TargetActor == nullptr)
	{
		return false;
	}

	UWorld* World = TargetActor->GetWorld();
	if(World == nullptr)
	{
		return false;
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams TraceParams("AttackCheck", true, Blackboard.GetOwner());
	TraceParams.AddIgnoredActor(ControlledPawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(TargetActor->GetCharacterStats().SightRange);
	
	TArray<FHitResult> Hits;
	World->SweepMultiByObjectType(Hits, ControlledPawn->GetActorLocation(), ControlledPawn->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape, TraceParams);

	TArray<AActor*> ActorsProcessed;
	const int32 HitsCount = Hits.Num();
	for(int32 i = 0; i < HitsCount; ++i)
	{
		AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(Hits[i].GetActor());
		if(InfomorphCharacter == nullptr || ActorsProcessed.Contains(InfomorphCharacter))
		{
			continue;
		}
		ActorsProcessed.Add(InfomorphCharacter);

		if(InfomorphCharacter->IsAttacking())
		{
			return true;
		}
	}

	return false;
}

void UInfomorph_IsTargetUnderAttack::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UInfomorph_IsTargetUnderAttack::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
