// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InfomorphWeapon.h"
#include "InfomorphShield.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.generated.h"

USTRUCT()
struct FCharacterStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Stats)
		float BaseConsciousness;
	UPROPERTY(EditAnywhere, Category = Stats)
		float BaseEnergy;
	UPROPERTY(EditAnywhere, Category = Stats)
		float EnergyRecoveryPerSecond;
	UPROPERTY(EditAnywhere, Category = Stats)
		float EnergyRestoreCooldown;
	UPROPERTY(EditAnywhere, Category = Stats)
		float ConsciousnessArmorWhenPossessed;
	UPROPERTY(EditAnywhere, Category = Stats)
		float DodgeSpeed;
	UPROPERTY(EditAnywhere, Category = Confusion)
		float ConfusionPossessedTime;
	UPROPERTY(EditAnywhere, Category = Confusion)
		float ConfusionUnpossessedTime;
	UPROPERTY(EditAnywhere, Category = Senses)
		float SightRange;
	UPROPERTY(EditAnywhere, Category = Senses)
		float HearRange;
	UPROPERTY(EditAnywhere, Category = Senses)
		float LooseTargetTimeout;
	UPROPERTY(EditAnywhere, Category = Costs)
		float LightAttackEnergyCost;
	UPROPERTY(EditAnywhere, Category = Costs)
		float HeavyAttackEnergyCost;
	UPROPERTY(EditAnywhere, Category = Costs)
		float SpecialAttackEnergyCost;
	UPROPERTY(EditAnywhere, Category = Costs)
		float DodgeEnergyCost;
	UPROPERTY(EditAnywhere, Category = Costs)
		float BlockEnergyCost;
	UPROPERTY(EditAnywhere, Category = Damage)
		float LightAttackDamage;
	UPROPERTY(EditAnywhere, Category = Damage)
		float HeavyAttackDamage;
	UPROPERTY(EditAnywhere, Category = Damage)
		float SpecialAttackDamage;
	UPROPERTY(EditAnywhere, Category = Cooldowns)
		float SpecialAttackCooldown;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, Category = Light)
		class UPointLightComponent* Light;

	UPROPERTY(EditAnywhere, Category = Interaction)
		class USphereComponent* InteractionSphere;

	UPROPERTY(EditAnywhere, Category = Weapon)
		TSubclassOf<AInfomorphWeapon> WeaponClass;
	UPROPERTY(EditAnywhere, Category = Shield)
		TSubclassOf<AInfomorphShield> ShieldClass;
	UPROPERTY(EditAnywhere, Category = Weapon)
		FName WeaponSocketName;
	UPROPERTY(EditAnywhere, Category = Shield)
		FName ShieldSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		FCharacterStats CharacterStats;

	AInfomorphWeapon* CurrentWeapon;

	AActor* CameraTarget;
	float LockedCameraTimer;
	bool bIsCameraLocked;

	USceneComponent* InteractionTarget;
	float InteractionTargetSetTimer;
	float InteractionRotateToTargetTimer;
	float InteractionMoveToTargetLerpTime;
	float InteractionRotateToTargetLerpTime;

	class UMaterialInstanceDynamic* MaterialInstance;

	FTimerHandle ConfusionTimerHandle;
	FTimerHandle DedigitalizeTimerHandle;

	FVector DodgeWorldDirection;
	FVector BeforeAttackDirection;
	FVector InitialLocation;

	float PossessionTime;
	float LastTimeTargetSeen;
	float LastActionTime;
	float LastSpecialAttackTime;
	float PrepareAttackTime;

	bool bIsInStealthMode;
	bool bIsLightAttack;
	bool bIsHeavyAttack;
	bool bIsSpecialAttack;
	bool bIsDodging;
	bool bIsDodgingZeroInput;
	bool bWasHit;
	bool bIsBlocking;

public:
	UPROPERTY(EditAnywhere, Category = AI)
		class UBehaviorTree* BehaviorTree;

protected:
	void ProcessCameraLocked(float DeltaSeconds);
	void ProcessInteractionTarget(float DeltaSeconds);

	void ConfusionEnd();

	void DestroyActor();
	void RestartLevel();

	bool IsTargetVisible(const FVector& Direction) const;

public:
	AInfomorphUE4Character();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void FellOutOfWorld(const class UDamageType& DamageType) override;

	virtual void StartBlock();
	virtual void EndBlock();
	virtual void Dodge(const FVector& DodgeDirection);
	virtual void EnterStealthMode();
	virtual void ExitStealthMode();
	virtual void Attack();
	virtual void HeavyAttack();
	virtual void SpecialAttack();
	virtual void SpecialAbility();
	virtual bool LockCameraOnTarget(AActor* Target);
	virtual void UnlockCamera();

	virtual void Dedigitalize();

	float GetPossessionChance(const FVector& PlayerLocation);
	void Confuse(float ConfusionTime, float Multiplier = 1.0f);
	void SetInteractionTarget(USceneComponent* NewInteractionTarget);
	float CalculateTargetYaw(const FRotator& CurrentRotation, const FRotator& TargetRotation, float LerpT) const;

	UFUNCTION(BlueprintNativeEvent, Category = Camera)
		void OnCameraLocked(AActor* NewTarget);
	UFUNCTION(BlueprintNativeEvent, Category = Camera)
		void OnCameraUnlocked(AActor* OldTarget);

	UFUNCTION(BlueprintCallable, Category = Attack)
		void EnableWeaponCollision();
	UFUNCTION(BlueprintCallable, Category = Attack)
		void DisableWeaponCollision();

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsInStealthMode() const { return bIsInStealthMode; }

	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsCameraLocked() const { return bIsCameraLocked; }

	UFUNCTION(BlueprintCallable, Category = Possession)
		FORCEINLINE bool IsConfused() const { return CharacterStats.bIsConfused; }

	UFUNCTION(BlueprintCallable, Category = Attack)
		FORCEINLINE bool IsLightAttack() const { return bIsLightAttack; }
	UFUNCTION(BlueprintCallable, Category = Attack)
		FORCEINLINE bool IsHeavyAttack() const { return bIsHeavyAttack; }
	UFUNCTION(BlueprintCallable, Category = Attack)
		FORCEINLINE bool IsSpecialAttack() const { return bIsSpecialAttack; }
	UFUNCTION(BlueprintCallable, Category = Attack)
		FORCEINLINE bool IsPreparingAttack() const { return PrepareAttackTime > 0.0f; }
	UFUNCTION(BlueprintCallable, Category = Damage)
		FORCEINLINE bool WasHit() const { return bWasHit; }
	UFUNCTION(BlueprintCallable, Category = Dodge)
		FORCEINLINE bool IsDodging() const { return bIsDodging; }
	UFUNCTION(BlueprintCallable, Category = Dodge)
		FORCEINLINE bool IsDodgingZeroInput() const { return bIsDodgingZeroInput; }
	UFUNCTION(BlueprintCallable, Category = Block)
		FORCEINLINE bool IsBlocking() const { return bIsBlocking; }
	UFUNCTION(BlueprintCallable, Category = Info)
		FORCEINLINE bool IsDead() const { return CharacterStats.CurrentConsciousness <= 0.0f; }

	FORCEINLINE bool IsActionsDisabled() const
	{
		return bIsLightAttack || bIsHeavyAttack || bIsDodging || bIsSpecialAttack || bWasHit || CharacterStats.bIsConfused || IsDead() || InteractionTarget != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = Attack) 
		void ResetAttacks()
	{
		bIsLightAttack = false;
		bIsHeavyAttack = false;
		bIsSpecialAttack = false;
		PrepareAttackTime = 0.0f;
	}

	UFUNCTION(BlueprintCallable, Category = Damage) 
		void ResetHit()
	{
		bWasHit = false;

		AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
		if(InfomorphAIController)
		{
			InfomorphAIController->ResumeBehaviorTree();
		}
	}

	UFUNCTION(BlueprintCallable, Category = Dodge) 
		void ResetDodging()
	{
		bIsDodging = false;
		bIsDodgingZeroInput = false;
	}

	UFUNCTION(BlueprintCallable, Category = Block)
		void ResetBlocking()
	{
		bIsBlocking = false;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		void ResetState()
	{
		bIsLightAttack =
			bIsHeavyAttack =
			bIsSpecialAttack =
			bIsDodging =
			bIsDodgingZeroInput =
			bWasHit =
			bIsBlocking = false;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE FVector GetInitialLocation() const { return InitialLocation; }

	UFUNCTION(BlueprintCallable, Category = Camera)
		FVector GetEyesLocation() const;

	UFUNCTION(BlueprintCallable, Category = Camera)
		FVector GetEyesDirection() const;

	UFUNCTION(BlueprintCallable, Category = Senses)
		float GetSightRange() const { return CharacterStats.SightRange; }
	UFUNCTION(BlueprintCallable, Category = Senses)
		float GetHearRange() const { return CharacterStats.HearRange; }
	//Returns 0 if special attack is ready and 1 if time remaining to activate special attack is equal to SpecialAttackCooldown specified in CharacterStats struct
	UFUNCTION(BlueprintCallable, Category = Stats)
		float GetRatioRemainingToActivateSpecialAttack() const
	{
		if(!IsValidLowLevel())
		{
			return 1.0f;
		}

		if(CharacterStats.SpecialAttackCooldown == 0.0f)
		{
			return 0.0f;
		}
		float Ratio = 1.0f - (GetWorld()->GetRealTimeSeconds() - LastSpecialAttackTime) / CharacterStats.SpecialAttackCooldown;
		return FMath::Clamp(Ratio, 0.0f, 1.0f);
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE float GetCurrentConsciousness() const
	{
		return CharacterStats.CurrentConsciousness;
	}
	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE float GetCurrentEnergy() const
	{
		return CharacterStats.CurrentEnergy;
	}
	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE float GetMaxConsciousness() const
	{
		return CharacterStats.BaseConsciousness;
	}
	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE float GetMaxEnergy() const
	{
		return CharacterStats.BaseEnergy;
	}
	UFUNCTION(BlueprintCallable, Category = Utility)
		FORCEINLINE UMaterialInstanceDynamic* GetMaterialInstance() const
	{
		return MaterialInstance;
	}

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AActor* GetCameraTarget() const { return CameraTarget; }
	FORCEINLINE const FCharacterStats& GetCharacterStats() const { return CharacterStats; }
	FORCEINLINE class USphereComponent* GetInteractionSphere() const { return InteractionSphere; }
};

