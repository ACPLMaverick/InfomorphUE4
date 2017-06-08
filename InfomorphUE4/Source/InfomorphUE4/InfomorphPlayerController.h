// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "GameFramework/PlayerController.h"
#include "InfomorphPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API AInfomorphPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float LookTimerThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Possession)
		float PossessingTime;

	float LastLookedTimer;
	float LastMovedTimer;

	class AInfomorphUE4Character* CharacterToPossess;
	float PossessingTimer;
	bool bIsPossessing;
	
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
	void PerformStartPossessing();
	void PerformStopPossessing();
	void PerformCameraLock();

	void PossessNewCharacter(class AInfomorphUE4Character* NewCharacter);

	AActor* GetActorInLookDirection(const FVector& EyesLocation, const FVector &Direction) const;

public:
	AInfomorphPlayerController();
	AInfomorphPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void SetupInputComponent() override;
	
	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE float GetLastLookedTimer() const { return LastLookedTimer; }
	FORCEINLINE float GetLastMovedTimer() const { return LastMovedTimer; }
	FORCEINLINE float GetLookTimerTreshold() const { return LookTimerThreshold; }
};
