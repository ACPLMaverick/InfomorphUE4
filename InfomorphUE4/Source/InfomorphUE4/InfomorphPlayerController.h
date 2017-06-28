// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/PlayerController.h"
#include "InfomorphSkillBase.h"
#include "InfomorphInteractable.h"
#include "Runtime/Engine/Classes/Components/ForceFeedbackComponent.h"
#include "InfomorphPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API AInfomorphPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Feedback)
		UForceFeedbackComponent* FeedbackComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float LookTimerThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skills)
		TArray<FSkillInfo> Skills;

	AInfomorphInteractable* CurrentInteractable;

	FVector VelocityBeforeJump;
	float LastLookedTimer;
	float LastMovedTimer;

	float MovementMultiplier;

	bool bRecentlyTriedToSwapCameraTarget;

	int32 CurrentSelectedSkillIndex;

	class UInfomorphTutorialWidget* CurrentTutorialWidget;

	FVector MoveInput;
	
protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void TurnAtRate(float Rate);
	void LookUp(float Value);
	void LookUpAtRate(float Rate);
	void PerformHeavyAttack();
	void PerformAttack();
	void PerformSpecialAttack();
	void PerformStartBlock();
	void PerformEndBlock();
	void PerformDodge();
	void PerformStealthMode();
	void PerformInteraction();
	void PerformSpecialAbility();
	void PerformSpecialPossessedCharacterAbility();
	void PerformCameraLock();
	void PerformJump();
	void PerformStartSkillUsage();
	void PerformStopSkillUsage();
	void PerformSkillSelectUp();
	void PerformSkillSelectDown();

	AActor* GetActorInLookDirection(const FVector& EyesLocation, const FVector &Direction, float MaxDistance) const;

	void LookForInteractables(float DeltaSeconds);
	void TryLockCamera(AActor* CurrentTarget, const FVector& InputDirection = FVector::ZeroVector);

public:
	AActor* GetNextActorInDirection(float MaxDistance, AActor* CurrentActor, const FVector& InputDirection);

	AInfomorphPlayerController();
	AInfomorphPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	virtual void Tick(float DeltaSeconds) override;

	void PossessNewCharacter(class AInfomorphUE4Character* NewCharacter);

	void InteractWithCurrentInteractable();

	void SetTutorialWidget(class UInfomorphTutorialWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = Noise)
		void MakeFootstepNoise();

	FORCEINLINE FVector GetVelocityBeforeJump() const { return VelocityBeforeJump; }
	FORCEINLINE float GetLastLookedTimer() const { return LastLookedTimer; }
	FORCEINLINE float GetLastMovedTimer() const { return LastMovedTimer; }
	FORCEINLINE float GetLookTimerTreshold() const { return LookTimerThreshold; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		void SetMovementMultiplier(float NewMultiplier)
	{
		MovementMultiplier = NewMultiplier;
	}

	FORCEINLINE void PlayFeedback(UForceFeedbackEffect* FeedbackEffect)
	{
		FeedbackComponent->Stop();
		FeedbackComponent->SetForceFeedbackEffect(FeedbackEffect);
		FeedbackComponent->Play();
	}

	FORCEINLINE void StopFeedback()
	{
		FeedbackComponent->Stop();
	}

	UFUNCTION(BlueprintCallable, Category = Skills)
		FORCEINLINE bool IsUsingSkill() const
	{
		UInfomorphSkillBase* Skill = GetCurrentSkill();
		return Skill != nullptr && Skill->IsBeingUsed();
	}

	UFUNCTION(BlueprintCallable, Category = Skills)
		FORCEINLINE UInfomorphSkillBase* GetCurrentSkill() const
	{
		return (CurrentSelectedSkillIndex < Skills.Num() && Skills.Num() > 0) ? Skills[CurrentSelectedSkillIndex].Skill : nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = Skills)
		FORCEINLINE int32 GetCurrentSkillIndex() const
	{
		return CurrentSelectedSkillIndex;
	}

	UFUNCTION(BlueprintCallable, Category = Interaction)
		FORCEINLINE bool IsInteractionPossible() const
	{
		return CurrentInteractable != nullptr && CurrentInteractable->IsInteractionPossible();
	}

	UFUNCTION(BlueprintCallable, Category = Interaction)
		FORCEINLINE AInfomorphInteractable* GetCurrentInteractable() const
	{
		return CurrentInteractable;
	}

	//Returns float from 0 to 1, where 1 means that whole cooldown remains and 0 means that skill is ready
	UFUNCTION(BlueprintCallable, Category = Skills)
	FORCEINLINE float GetCurrentSkillRemainingRatio() const
	{
		if(CurrentSelectedSkillIndex >= Skills.Num() || Skills[CurrentSelectedSkillIndex].Skill == nullptr)
		{
			return 1.0f;
		}

		UInfomorphSkillBase* Skill = Skills[CurrentSelectedSkillIndex].Skill;
		float TimeElapsedSinceLastUsage = FPlatformTime::Seconds() - Skill->GetLastUsedTime();
		float Ratio = 1.0f - TimeElapsedSinceLastUsage / Skill->GetCooldown();
		return FMath::Clamp(Ratio, 0.0f, 1.0f);
	}
};
