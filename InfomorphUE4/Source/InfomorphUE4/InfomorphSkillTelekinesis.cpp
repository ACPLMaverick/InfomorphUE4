// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphSkillTelekinesis.h"
#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphTelekineticPawn.h"
#include "Curves/CurveFloat.h"

void UInfomorphSkillTelekinesis::OnTelekinesisCompleted()
{

}

void UInfomorphSkillTelekinesis::Tick(float DeltaSeconds)
{
	if(IsBeingUsed())
	{
		TelekinesisTimer += DeltaSeconds;
		if(TelekinesisTimer >= TelekinesisTime)
		{
			TelekineticPawn->MoveToTarget(TelekineticPawnHeightCurve->GetFloatValue(1.0f), InfomorphPC);
			TelekineticPawn = nullptr;
		}
	}

	TelekinesisTimer = FMath::Clamp(TelekinesisTimer, 0.0f, TelekinesisTime);

	if(TelekineticPawn != nullptr)
	{
		float Alpha = TelekinesisTime == 0.0f ? TelekinesisTimer : TelekinesisTimer / TelekinesisTime;
		float ZOffset = TelekineticPawnHeightCurve->GetFloatValue(Alpha);
		TelekineticPawn->SetActorLocation(TelekineticBaseLocation + FVector::UpVector * ZOffset);
	}
}

void UInfomorphSkillTelekinesis::StartUsing(AInfomorphPlayerController* InfomorphPlayerController)
{
	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(InfomorphPlayerController->GetPawn());
	if(Character == nullptr || Character->GetCameraTarget() == nullptr || !Character->GetCameraTarget()->IsA<AInfomorphTelekineticPawn>())
	{
		StopUsing();
		return;
	}

	TelekineticPawn = Cast<AInfomorphTelekineticPawn>(Character->GetCameraTarget());
	TelekineticBaseLocation = TelekineticPawn->GetActorLocation();

	TelekinesisTimer = 0.0f;
	InfomorphPC = InfomorphPlayerController;
	InfomorphPC->SetMovementMultiplier(0.0f);
	InfomorphPC->GetWorldTimerManager().SetTimer(TelekinesisCompletedTimerHandle, this, &UInfomorphSkillTelekinesis::OnTelekinesisCompleted, TelekinesisTime);
	InfomorphPC->PlayFeedback(VibrationEffect);
}

void UInfomorphSkillTelekinesis::StopUsing()
{
	if(InfomorphPC == nullptr)
	{
		return;
	}

	if(TelekineticPawn != nullptr)
	{
		TelekineticPawn->Drop();
		TelekineticPawn = nullptr;
	}

	InfomorphPC->SetMovementMultiplier(1.0f);
	InfomorphPC->GetWorldTimerManager().ClearTimer(TelekinesisCompletedTimerHandle);
	TelekinesisCompletedTimerHandle.Invalidate();
	InfomorphPC->StopFeedback();
}
