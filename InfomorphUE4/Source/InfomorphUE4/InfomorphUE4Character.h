// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InfomorphUE4Character.generated.h"

USTRUCT()
struct FCharacterStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		float BaseConsciousness;
	UPROPERTY(EditAnywhere)
		float BaseEnergy;
	UPROPERTY(EditAnywhere)
		float ConfusionPossessedTime;
	UPROPERTY(EditAnywhere)
		float ConfusionUnpossessedTime;
	UPROPERTY(EditAnywhere)
		float SightRange;
	UPROPERTY(EditAnywhere)
		float HearRange;
	UPROPERTY(EditAnywhere)
		float LooseTargetTimeout;

	float CurrentConsciousness;
	float CurrentEnergy;
	bool bIsConfused;

	FCharacterStats();

	void Initialize();
};

UCLASS(config=Game)
class AInfomorphUE4Character : public ACharacter
{
	GENERATED_BODY()

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		FCharacterStats CharacterStats;

	bool bIsInStealthMode;

	AActor* CameraTarget;
	float LockedCameraTimer;
	bool bIsCameraLocked;

	FTimerHandle ConfusionTimerHandle;

	float LastTimeTargetSeen;

protected:
	void ProcessCameraLocked(float DeltaSeconds);
	void ConfusionEnd();

	bool IsTargetVisible(const FVector& Direction) const;

public:
	AInfomorphUE4Character();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void StartBlock();
	virtual void EndBlock();
	virtual void Dodge();
	virtual void EnterStealthMode();
	virtual void ExitStealthMode();
	virtual void Attack();
	virtual void HeavyAttack();
	virtual void SpecialAttack();
	virtual void SpecialAbility();
	virtual bool LockCameraOnTarget(AActor* Target);
	virtual void UnlockCamera();

	float GetPossessionChance(const FVector& PlayerLocation);
	void Confuse(float ConfusionTime, float Multiplier = 1.0f);

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsInStealthMode() const { return bIsInStealthMode; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsCameraLocked() const { return bIsCameraLocked; }

	UFUNCTION(BlueprintCallable, Category = Possession)
		FORCEINLINE bool IsConfused() const { return CharacterStats.bIsConfused; }

	UFUNCTION(BlueprintCallable, Category = Camera)
		FVector GetEyesLocation() const;

	UFUNCTION(BlueprintCallable, Category = Camera)
		FVector GetEyesDirection() const;

	UFUNCTION(BlueprintCallable, Category = Info)
		float GetSightRange() const { return CharacterStats.SightRange; }
	UFUNCTION(BlueprintCallable, Category = Info)
		float GetHearRange() const { return CharacterStats.HearRange; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AActor* GetCameraTarget() const { return CameraTarget; }
	FORCEINLINE const FCharacterStats& GetCharacterStats() const { return CharacterStats; }
};

