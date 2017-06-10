// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "InfomorphSkillBase.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, BlueprintType, abstract)
class INFOMORPHUE4_API UInfomorphSkillBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds);

	virtual void StartUsing(class AInfomorphPlayerController* InfomorphPlayerController);
	virtual void StopUsing();

	FORCEINLINE virtual bool IsBeingUsed() const
	{
		return false;
	}
};

USTRUCT()
struct INFOMORPHUE4_API FSkillInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FName SkillName;
	UPROPERTY(EditAnywhere)
		TSubclassOf<UInfomorphSkillBase> SkillClass;
	UPROPERTY(VisibleAnywhere)
		UInfomorphSkillBase* Skill;

	void Initialize();
};

UCLASS(Blueprintable, BlueprintType)
class INFOMORPHUE4_API UInfomorphSkillPossession : public UInfomorphSkillBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Feedback)
		UForceFeedbackEffect* BuildUpVibrationEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Feedback)
		UForceFeedbackEffect* PossessingVibrationEffect;
	UPROPERTY(EditAnywhere, Category = Possession)
		float BuildUpTime;
	UPROPERTY(EditAnywhere, Category = Possession)
		float PossessionTime;

	class AInfomorphPlayerController* InfomorphPC;
	class AInfomorphUE4Character* CharacterToPossess;

	FTimerHandle BuildUpTimerHandle;
	FTimerHandle PossessingTimerHandle;

	float BuildUpTimer;

protected:
	void OnBuildUpTimerCompleted();
	void OnPossessionTimerCompleted();

public:
	virtual void Tick(float DeltaSeconds);

	virtual void StartUsing(class AInfomorphPlayerController* InfomorphPlayerController) override;
	virtual void StopUsing() override;

	FORCEINLINE virtual bool IsBeingUsed() const override
	{
		return BuildUpTimerHandle.IsValid() || PossessingTimerHandle.IsValid();
	}
};