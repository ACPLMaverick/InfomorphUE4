// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphSkillBase.h"
#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"

//Skill base methods
void UInfomorphSkillBase::Tick(float DeltaSeconds)
{

}

void UInfomorphSkillBase::StartUsing(AInfomorphPlayerController* InfomorphPlayerController)
{

}

void UInfomorphSkillBase::StopUsing()
{

}

//SkillInfo methods
void FSkillInfo::Initialize()
{
	Skill = NewObject<UInfomorphSkillBase>((UObject*)GetTransientPackage(), SkillClass);
}

//Possession skill methods
void UInfomorphSkillPossession::OnBuildUpTimerCompleted()
{
	if(InfomorphPC == nullptr)
	{
		return;
	}

	InfomorphPC->PlayFeedback(PossessingVibrationEffect);
	InfomorphPC->SetViewTargetWithBlend(CharacterToPossess, PossessionTime, EViewTargetBlendFunction::VTBlend_Linear);
	InfomorphPC->GetWorldTimerManager().SetTimer(PossessingTimerHandle, this, &UInfomorphSkillPossession::OnPossessionTimerCompleted, PossessionTime);
}

void UInfomorphSkillPossession::OnPossessionTimerCompleted()
{
	InfomorphPC->StopFeedback();
	InfomorphPC->PossessNewCharacter(CharacterToPossess);
	InfomorphPC->ResetLookMultiplier();
	InfomorphPC->ResetMovementMultiplier();
}

void UInfomorphSkillPossession::Tick(float DeltaSeconds)
{
	if(BuildUpTimerHandle.IsValid())
	{
		BuildUpTimer += DeltaSeconds;
		if(InfomorphPC != nullptr)
		{
			InfomorphPC->SetMovementMultiplier(1.0f - BuildUpTimer / (BuildUpTime * 0.8f));
		}
	}
}

void UInfomorphSkillPossession::StartUsing(AInfomorphPlayerController* InfomorphPlayerController)
{
	if(IsBeingUsed() || InfomorphPlayerController == nullptr)
	{
		return;
	}

	InfomorphPC = InfomorphPlayerController;

	if(InfomorphPC != nullptr)
	{
		AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(InfomorphPC->GetPawn());
		if(CurrentlyPossessedCharacter != nullptr)
		{
			CharacterToPossess = Cast<AInfomorphUE4Character>(CurrentlyPossessedCharacter->GetCameraTarget());
			if(CharacterToPossess != nullptr)
			{
				float PossessionChance = CharacterToPossess->GetPossessionChance(CurrentlyPossessedCharacter->GetActorLocation());
				if(PossessionChance < 1.0f)
				{
					float Random = FMath::RandRange(0.0f, 1.0f);
					if(Random > PossessionChance)
					{
						return;
					}
				}
				InfomorphPC->SetLookMultiplier(0.0f);
				InfomorphPC->PlayFeedback(BuildUpVibrationEffect);
				BuildUpTimer = 0.0f;
				InfomorphPC->GetWorldTimerManager().SetTimer(BuildUpTimerHandle, this, &UInfomorphSkillPossession::OnBuildUpTimerCompleted, BuildUpTime);
			}
		}
	}
}

void UInfomorphSkillPossession::StopUsing()
{
	if(InfomorphPC == nullptr)
	{
		return;
	}

	InfomorphPC->ResetLookMultiplier();
	InfomorphPC->ResetMovementMultiplier();
	InfomorphPC->StopFeedback();
	InfomorphPC->GetWorldTimerManager().ClearTimer(BuildUpTimerHandle);
	InfomorphPC->GetWorldTimerManager().ClearTimer(PossessingTimerHandle);

	AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(InfomorphPC->GetPawn());
	if(CurrentlyPossessedCharacter != nullptr)
	{
		InfomorphPC->SetViewTargetWithBlend(CurrentlyPossessedCharacter, PossessionTime, EViewTargetBlendFunction::VTBlend_Linear);
	}
}
