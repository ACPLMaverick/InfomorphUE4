// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_GeneratePatrolLocation.h"
#include "InfomorphUE4Character.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UInfomorph_GeneratePatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(OwnerComp.GetAIOwner());
	if(InfomorphAIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(InfomorphAIController->GetPawn());
	if(InfomorphCharacter == nullptr)
	{
		LogOnScreen("?");
		return EBTNodeResult::Failed;
	}

	FVector CharacterLocation = InfomorphCharacter->GetInitialLocation();

	FVector ToPatrolLocation(1.0f, 0.0f, 0.0f);
	ToPatrolLocation = FRotator(0.0f, FMath::RandRange(0.0f, 360.0f), 0.0f).RotateVector(ToPatrolLocation);

	FVector PatrolLocation = CharacterLocation + ToPatrolLocation * FMath::RandRange(MinDistanceToPatrolLocation, MaxDistanceToPatrolLocation);

	OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolLocation", PatrolLocation);

	LogOnScreen("!");
	return EBTNodeResult::Succeeded;
}

void UInfomorph_GeneratePatrolLocation::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}
