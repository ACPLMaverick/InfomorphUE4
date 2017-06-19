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
	friend struct FSkillInfo;

	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		float Cooldown;

	float LastUsedTime;

public:
	virtual void Tick(float DeltaSeconds);

	virtual void StartUsing(class AInfomorphPlayerController* InfomorphPlayerController);
	virtual void StopUsing();

	UFUNCTION(BlueprintCallable, Category = Skill)
	virtual FORCEINLINE bool IsBeingUsed() const
	{
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
	virtual FORCEINLINE bool CanBeUsed() const
	{
		float CurrentRealSeconds = FPlatformTime::Seconds();
		return CurrentRealSeconds - LastUsedTime > Cooldown;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
	FORCEINLINE float GetLastUsedTime() const
	{
		return LastUsedTime;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
	FORCEINLINE float GetCooldown() const
	{
		return Cooldown;
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
	FTimerHandle CheckIfPossessableTimerHandle;

	float BuildUpTimer;
	float PossessionTimer;

	class AInfomorphBaseAIController* TargetController;

protected:
	void OnBuildUpTimerCompleted();
	void OnPossessionTimerCompleted();
	void OnCheckIfPossessableTimerCompleted();

public:
	virtual void Tick(float DeltaSeconds);

	virtual void StartUsing(class AInfomorphPlayerController* InfomorphPlayerController) override;
	virtual void StopUsing() override;

	UFUNCTION(BlueprintCallable, Category = Skill)
	virtual FORCEINLINE bool IsBeingUsed() const override
	{
		return BuildUpTimerHandle.IsValid() || PossessingTimerHandle.IsValid();
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
	FORCEINLINE float GetPossessionTime() const
	{
		return PossessionTime;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
	FORCEINLINE AInfomorphUE4Character* GetPossessedCharacter() const
	{
		return CharacterToPossess;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
		FORCEINLINE float GetPossessionTimer() const
	{
		return PossessionTimer;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
		FORCEINLINE float GetBuildUpTime() const
	{
		return BuildUpTime;
	}

	UFUNCTION(BlueprintCallable, Category = Skill)
		FORCEINLINE float GetBuildUpTimer() const
	{
		return BuildUpTimer;
	}
};