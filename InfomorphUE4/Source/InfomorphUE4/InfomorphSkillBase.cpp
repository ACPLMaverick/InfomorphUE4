// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphSkillBase.h"
#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphBaseAIController.h"
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
	if(Skill != nullptr)
	{
		Skill->LastUsedTime = -Skill->Cooldown;
	}
}

//Possession skill methods
void UInfomorphSkillPossession::OnBuildUpTimerCompleted()
{
	if(InfomorphPC == nullptr)
	{
		return;
	}

	InfomorphPC->PlayFeedback(PossessingVibrationEffect);
	//InfomorphPC->SetViewTargetWithBlend(CharacterToPossess, PossessionTime, EViewTargetBlendFunction::VTBlend_Cubic);
	InfomorphPC->GetWorldTimerManager().SetTimer(CheckIfPossessableTimerHandle, this, &UInfomorphSkillPossession::OnCheckIfPossessableTimerCompleted, 0.5f * PossessionTime);
	PossessionTimer = 0.0f;
}

void UInfomorphSkillPossession::OnPossessionTimerCompleted()
{
	InfomorphPC->StopFeedback();
	InfomorphPC->PossessNewCharacter(CharacterToPossess);
	InfomorphPC->ResetLookMultiplier();
	InfomorphPC->ResetMovementMultiplier();
}

void UInfomorphSkillPossession::OnCheckIfPossessableTimerCompleted()
{
	if(InfomorphPC == nullptr)
	{
		StopUsing();
		return;
	}

	AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(InfomorphPC->GetPawn());
	if(CurrentlyPossessedCharacter != nullptr)
	{
		bool bIsPossessable = false;
		float PossessionChance = CharacterToPossess->GetPossessionChance(CurrentlyPossessedCharacter->GetActorLocation());
		bIsPossessable = FMath::RandRange(0.0f, 1.0f) <= PossessionChance;
		if(bIsPossessable)
		{
			float TimerTime = 0.5f * PossessionTime;
			InfomorphPC->SetViewTargetWithBlend(CharacterToPossess, 0.9f * TimerTime, EViewTargetBlendFunction::VTBlend_Linear);
			InfomorphPC->GetWorldTimerManager().SetTimer(PossessingTimerHandle, this, &UInfomorphSkillPossession::OnPossessionTimerCompleted, TimerTime);
		}
		else
		{
			CurrentlyPossessedCharacter->EventPossessionFailed(CharacterToPossess);
			CurrentlyPossessedCharacter->Confuse(CurrentlyPossessedCharacter->GetCharacterStats().ConfusionPossessedTime, 0.5f);
			StopUsing();
		}
	}
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
	if(PossessingTimerHandle.IsValid() || CheckIfPossessableTimerHandle.IsValid())
	{
		PossessionTimer += DeltaSeconds;
	}
}

void UInfomorphSkillPossession::StartUsing(AInfomorphPlayerController* InfomorphPlayerController)
{
	if(IsBeingUsed() || InfomorphPlayerController == nullptr)
	{
		return;
	}

	if(!CanBeUsed())
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
			if(CharacterToPossess != nullptr && !CharacterToPossess->IsDead())
			{
				TargetController = Cast<AInfomorphBaseAIController>(CharacterToPossess->GetController());
				if(TargetController != nullptr)
				{
					LogOnScreen("Pause BT");
					TargetController->PauseBehaviorTree("Possession");
				}

				LastUsedTime = FPlatformTime::Seconds();

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
	InfomorphPC->GetWorldTimerManager().ClearTimer(CheckIfPossessableTimerHandle);
	InfomorphPC->SetViewTargetWithBlend(InfomorphPC->GetPawn(), PossessionTime, EViewTargetBlendFunction::VTBlend_Cubic);
}
