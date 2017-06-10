// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InfomorphUE4Character.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Possession)
		float ConfusionPossessedTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Possession)
		float ConfusionUnPossessedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
		float BaseConsciousness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Info)
		float BaseEnergy;

	float CurrentConsciousness;
	float CurrentEnergy;

	bool bIsInStealthMode;

	AActor* CameraTarget;
	float LockedCameraTimer;
	bool bIsCameraLocked;

	FTimerHandle ConfusionTimerHandle;
	bool bIsConfused;

protected:
	void ProcessCameraLocked(float DeltaSeconds);
	void ConfusionEnd();

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

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsInStealthMode() const { return bIsInStealthMode; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsCameraLocked() const { return bIsCameraLocked; }

	UFUNCTION(BlueprintCallable, Category = Possession)
		FORCEINLINE bool IsConfused() const { return bIsConfused; }

	UFUNCTION(BlueprintCallable, Category = Camera)
		FVector GetEyesLocation() const;

	UFUNCTION(BlueprintCallable, Category = Camera)
		FVector GetEyesDirection() const;

	UFUNCTION(BlueprintCallable, Category = Info)
		float GetSightRange() const { return 1000.0f; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AActor* GetCameraTarget() const { return CameraTarget; }
};

