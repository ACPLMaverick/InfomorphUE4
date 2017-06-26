// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InfomorphWeapon.h"
#include "InfomorphShield.h"
#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Patrol UMETA(DisplayName = "Patrol"),
	TargetLocked UMETA(DisplayName = "Target Locked")
};

USTRUCT(Blueprintable, BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BaseConsciousness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BaseEnergy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float EnergyRecoveryPerSecond;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float EnergyRestoreCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float ConsciousnessArmorWhenPossessed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float ConsciousnessPercentPossessable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float DodgeSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Confusion)
		float ConfusionPossessedTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Confusion)
		float ConfusionUnpossessedTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Senses)
		float SightRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Senses)
		float HearRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Senses)
		float LooseTargetTimeout;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Costs)
		float LightAttackEnergyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Costs)
		float HeavyAttackEnergyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Costs)
		float SpecialAttackEnergyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Costs)
		float DodgeEnergyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Costs)
		float BlockEnergyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		float LightAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		float HeavyAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		float SpecialAttackDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cooldowns)
		float SpecialAttackCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		bool bCanEverDodge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float MaxSpeed;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
		class USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
		class USoundBase* DeathSound;

	AInfomorphWeapon* CurrentWeapon;
	AInfomorphShield* CurrentShield;

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
	bool bWantsToJump;

	bool bShieldBroken;

	bool bWantsToLightAttack;
	bool bWantsToHeavyAttack;
	bool bWantsToSpecialAttack;

	bool bAttackQueueEnabled;

	bool bIsFalling;
	float FallingTimer;

	float CombatModeCheckTimer;
	bool bIsInCombatMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		EMovementState MovementState;

public:
	UPROPERTY(EditAnywhere, Category = AI)
		class UBehaviorTree* BehaviorTree;

protected:
	void ProcessCameraLocked(float DeltaSeconds);
	void ProcessInteractionTarget(float DeltaSeconds);
	void ProcessPossessionMaterial(float DeltaSeconds);
	void CheckIfInCombatMode();
	void ProcessFalling(float DeltaSeconds);

	void DestroyActor();

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

	UFUNCTION(BlueprintCallable, Category = Possession)
		float GetPossessionChance(const FVector& PlayerLocation);

	void Confuse(float ConfusionTime, float Multiplier = 1.0f);
	void ConfusionEnd();
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
		FORCEINLINE FVector GetDodgeDirection() const
	{
		return DodgeWorldDirection;
	}
	UFUNCTION(BlueprintCallable, Category = Dodge)
		FORCEINLINE bool IsDodgingZeroInput() const { return bIsDodgingZeroInput; }
	UFUNCTION(BlueprintCallable, Category = Block)
		FORCEINLINE bool IsBlocking() const { return bIsBlocking; }
	UFUNCTION(BlueprintCallable, Category = Block)
		FORCEINLINE bool IsShieldBroken() const { return bShieldBroken; }
	UFUNCTION(BlueprintCallable, Category = Info)
		FORCEINLINE bool IsDead() const { return CharacterStats.CurrentConsciousness <= 0.0f; }
	UFUNCTION(BlueprintCallable, Category = Info)
		FORCEINLINE bool IsInCombatMode() const { return bIsInCombatMode; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool WantsToJump() const { return bWantsToJump; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsFalling() const { return bIsFalling; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE bool IsFallingFromHigh() const { return FallingTimer >= 1.1f; }
	UFUNCTION(BlueprintCallable, Category = Movement)
		FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

	FORCEINLINE bool IsActionsDisabled() const
	{
		return bShieldBroken || bIsDodging || bWasHit || CharacterStats.bIsConfused || IsDead() || InteractionTarget != nullptr;
	}

	FORCEINLINE bool IsAttacking() const
	{
		return bIsLightAttack || bIsHeavyAttack || bIsSpecialAttack;
	}

	UFUNCTION(BlueprintCallable, Category = Block)
		void ResetShieldBroken()
	{
		bShieldBroken = false;
	}

	UFUNCTION(BlueprintCallable, Category = Attack) 
		void ResetAttacks()
	{
		bIsLightAttack = false;
		bIsHeavyAttack = false;
		bIsSpecialAttack = false;
		PrepareAttackTime = 0.0f;

		if(bWantsToLightAttack)
		{
			Attack();
		}
		if(bWantsToHeavyAttack)
		{
			HeavyAttack();
		}
		if(bWantsToSpecialAttack)
		{
			SpecialAttack();
		}
		bWantsToHeavyAttack = bWantsToLightAttack = bWantsToSpecialAttack = false;
	}

	UFUNCTION(BlueprintCallable, Category = Movement)
		void SetWantsToJump(bool bNewWantsToJump)
	{
		bWantsToJump = bNewWantsToJump;
	}

	UFUNCTION(BlueprintCallable, Category = Attack)
		void SetAttackQueueEnabled(bool bEnabled)
	{
		bAttackQueueEnabled = bEnabled;
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

	UFUNCTION(BlueprintCallable, Category = Attack)
		void PlayWeaponSound()
	{
		CurrentWeapon->PlayAttackSound();
	}

	UFUNCTION(BlueprintCallable, Category = Dodge) 
		void ResetDodging()
	{
		bIsDodging = false;
		bIsDodgingZeroInput = false;
		UnCrouch();
	}

	UFUNCTION(BlueprintCallable, Category = Block)
		void ResetBlocking()
	{
		bIsBlocking = false;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		void ResetState()
	{
		ExitStealthMode();
		ResetDodging();
		ResetBlocking();
		ResetHit();
		SetWantsToJump(false);
		ResetAttacks();
		ResetAttacks();
	}

	UFUNCTION(BlueprintCallable, Category = Movement)
		void SetMovementState(EMovementState NewMovementState)
	{
		MovementState = NewMovementState;
	}

	UFUNCTION(BlueprintImplementableEvent, Category = Stats)
		void EventNotEnoughEnergy();

	UFUNCTION(BlueprintImplementableEvent, Category = Stats)
		void EventCooldownNotFinishedSkill();

	UFUNCTION(BlueprintImplementableEvent, Category = Stats)
		void EventCooldownNotFinishedSpecial();

	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE FVector GetInitialLocation() const { return InitialLocation; }

	UFUNCTION(BlueprintImplementableEvent, Category = Possession)
		void EventPossessionFailed(AInfomorphUE4Character* CharacterWantedToPossess);

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

	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE float GetCurrentConsciousnessPercent() const
	{
		return CharacterStats.CurrentConsciousness / CharacterStats.BaseConsciousness;
	}
	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE float GetCurrentEnergyPercent() const
	{
		return CharacterStats.CurrentEnergy / CharacterStats.BaseEnergy;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		FORCEINLINE bool GetIsPossessable(const FVector& PlayerLocation)
	{
		return (GetPossessionChance(PlayerLocation) > 0.0f);
	}

	UFUNCTION(BlueprintCallable, Category = Weapon)
		FORCEINLINE AInfomorphWeapon* GetCurrentWeapon()
	{
		return CurrentWeapon;
	}

	UFUNCTION(BlueprintCallable, Category = Shield)
		FORCEINLINE AInfomorphShield* GetCurrentShield()
	{
		return CurrentShield;
	}


	UFUNCTION(BlueprintCallable, Category = Stats)
		void SetCurrentConsciousness(float value)
	{
		CharacterStats.CurrentConsciousness = value;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		void SetCurrentEnergy(float value)
	{
		CharacterStats.CurrentEnergy = value;
	}


	UFUNCTION(BlueprintCallable, Category = Stats)
		void AddCurrentConsciousness(float value)
	{
		CharacterStats.CurrentConsciousness += value;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
		void AddCurrentEnergy(float value)
	{
		CharacterStats.CurrentEnergy += value;
	}


	UFUNCTION(BlueprintCallable, Category = Utility)
		FORCEINLINE AActor* GetCameraTarget() const { return CameraTarget; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE const FCharacterStats& GetCharacterStats() const { return CharacterStats; }
	FORCEINLINE class USphereComponent* GetInteractionSphere() const { return InteractionSphere; }
};