// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/PlayerController.h"
#include "InfomorphSkillBase.h"
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

	float LastLookedTimer;
	float LastMovedTimer;

	float MovementMultiplier;
	float LookMultiplier;

	int32 CurrentSelectedSkillIndex;
	
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

	AActor* GetActorInLookDirection(const FVector& EyesLocation, const FVector &Direction) const;

public:
	AInfomorphPlayerController();
	AInfomorphPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	virtual void Tick(float DeltaSeconds) override;

	void PossessNewCharacter(class AInfomorphUE4Character* NewCharacter);

	FORCEINLINE float GetLastLookedTimer() const { return LastLookedTimer; }
	FORCEINLINE float GetLastMovedTimer() const { return LastMovedTimer; }
	FORCEINLINE float GetLookTimerTreshold() const { return LookTimerThreshold; }

	FORCEINLINE void SetMovementMultiplier(float Multiplier)
	{
		MovementMultiplier = FMath::Clamp(Multiplier, 0.0f, 2.0f);
	}

	FORCEINLINE void ResetMovementMultiplier()
	{
		MovementMultiplier = 1.0f;
	}

	FORCEINLINE float GetMovementMultiplier() const
	{
		return MovementMultiplier;
	}

	FORCEINLINE void SetLookMultiplier(float Multiplier)
	{
		LookMultiplier = FMath::Clamp(Multiplier, 0.0f, 2.0f);
	}

	FORCEINLINE void ResetLookMultiplier()
	{
		LookMultiplier = 1.0f;
	}

	FORCEINLINE float GetLookMultiplier() const
	{
		return LookMultiplier;
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

	FORCEINLINE bool IsUsingSkill() const
	{
		return Skills.IsValidIndex(CurrentSelectedSkillIndex) && Skills[CurrentSelectedSkillIndex].Skill != nullptr && Skills[CurrentSelectedSkillIndex].Skill->IsBeingUsed();
	}
};
