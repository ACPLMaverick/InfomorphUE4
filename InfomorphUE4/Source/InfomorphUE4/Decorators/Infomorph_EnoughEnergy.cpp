// Fill out your copyright notice in the Description page of Project Settings.

#include "Infomorph_EnoughEnergy.h"
#include "InfomorphUE4Character.h"

bool UInfomorph_EnoughEnergy::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if(OwnerComp.GetAIOwner() == nullptr)
	{
		return false;
	}

	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(OwnerComp.GetAIOwner()->GetPawn());
	
	if(Character == nullptr)
	{
		return false;
	}

	return Evaluate(Character);
}

FString UInfomorph_EnoughEnergy::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: Energy >= %.2f"), *Super::GetStaticDescription(), MinimumEnergyLevel);
}

bool UInfomorph_EnoughEnergy::Evaluate(AInfomorphUE4Character* OwnerCharacter) const
{
	float Energy = OwnerCharacter->GetCurrentEnergy();
	return Energy >= MinimumEnergyLevel;
}

void UInfomorph_EnoughEnergy::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UInfomorph_EnoughEnergy::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
