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
	bool bIsInStealthMode;

	bool bIsCameraLocked;
	AActor* CameraTarget;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UArrowComponent* EyesArrow;

protected:
	void ProcessCameraLocked(float DeltaSeconds);

public:
	AInfomorphUE4Character();

	virtual void Tick(float DeltaSeconds);

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

	UFUNCTION(BlueprintCallable, Category = Movement)
	FORCEINLINE bool IsInStealthMode() const { return bIsInStealthMode; }

	UFUNCTION(BlueprintCallable, Category = Movement)
	FORCEINLINE bool IsCameraLocked() const { return bIsCameraLocked; }

	UFUNCTION(BlueprintCallable, Category = Camera)
	FVector GetEyesLocation() const;

	UFUNCTION(BlueprintCallable, Category = Camera)
	FVector GetEyesDirection() const;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AActor* GetCameraTarget() const { return CameraTarget; }
};

