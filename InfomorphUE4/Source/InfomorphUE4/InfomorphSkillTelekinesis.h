// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InfomorphSkillBase.h"
#include "InfomorphSkillTelekinesis.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API UInfomorphSkillTelekinesis : public UInfomorphSkillBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Feedback)
		UForceFeedbackEffect* VibrationEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Telekinesis)
		class UCurveFloat* TelekineticPawnHeightCurve;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Telekinesis)
		float TelekinesisTime;

	class AInfomorphPlayerController* InfomorphPC;
	class AInfomorphTelekineticPawn* TelekineticPawn;

	FTimerHandle TelekinesisCompletedTimerHandle;

	FVector TelekineticBaseLocation;
	float TelekinesisTimer;

protected:
	void OnTelekinesisCompleted();

public:
	virtual void Tick(float DeltaSeconds);

	virtual void StartUsing(class AInfomorphPlayerController* InfomorphPlayerController) override;
	virtual void StopUsing() override;

	UFUNCTION(BlueprintCallable, Category = Skill)
		virtual FORCEINLINE bool IsBeingUsed() const override
	{
		return TelekinesisCompletedTimerHandle.IsValid() && TelekineticPawn != nullptr;
	}
};
