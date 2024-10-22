// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "InfomorphUE4GameMode.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

FCharacterStats::FCharacterStats()
{
	BaseConsciousness = 80.0f;
	BaseEnergy = 100.0f;
	EnergyRecoveryPerSecond = 1.0f;
	EnergyRestoreCooldown = 2.0f;
	ConsciousnessArmorWhenPossessed = 0.3f;
	DodgeSpeed = 1.0f;

	ConfusionPossessedTime = 2.0f;
	ConfusionUnpossessedTime = 3.0f;
	
	SightRange = 1000.0f;
	HearRange = 750.0f;
	LooseTargetTimeout = 2.0f;

	LightAttackEnergyCost = 28.0f;
	HeavyAttackEnergyCost = 40.0f;
	SpecialAttackEnergyCost = 80.0f;
	DodgeEnergyCost = 40.0f;
	BlockEnergyCost = 1.0f;	//Will be multiplied by Damage caused by attack

	LightAttackDamage = 15.0f;
	HeavyAttackDamage = 40.0f;
	SpecialAttackDamage = 40.0f;

	SpecialAttackCooldown = 10.0f;

	bCanEverDodge = true;
	bCanEverCrouch = false;
}

void FCharacterStats::Initialize()
{
	CurrentConsciousness = BaseConsciousness;
	CurrentEnergy = BaseEnergy;
	bIsConfused = false;
}

//////////////////////////////////////////////////////////////////////////
// AInfomorphUE4Character

void AInfomorphUE4Character::ProcessCameraLocked(float DeltaSeconds)
{
	LockedCameraTimer += DeltaSeconds;

	AInfomorphUE4Character* TargetCharacter = Cast<AInfomorphUE4Character>(CameraTarget);
	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if((TargetCharacter != nullptr && TargetCharacter->IsDead()))
	{
		if(InfomorphPC != nullptr)
		{
			AActor* NewTarget = InfomorphPC->GetNextActorInDirection(CharacterStats.SightRange, TargetCharacter, FVector::RightVector);
			if(NewTarget == nullptr)
			{
				NewTarget = InfomorphPC->GetNextActorInDirection(CharacterStats.SightRange, TargetCharacter, -FVector::RightVector);
			}

			if(NewTarget != nullptr)
			{
				LockCameraOnTarget(NewTarget);
			}
			else
			{
				UnlockCamera();
			}
		}
		else
		{
			UnlockCamera();
		}
		return;
	}
	if(IsDead())
	{
		UnlockCamera();
		return;
	}

	FVector Direction = CameraTarget->GetActorLocation() - GetEyesLocation();
	
	if(Direction.Size() > 1.5f * CharacterStats.SightRange)
	{
		UnlockCamera();
		return;
	}

	if(IsTargetVisible(CameraTarget))
	{
		LastTimeTargetSeen = FPlatformTime::Seconds();
	}

	if(FPlatformTime::Seconds() - LastTimeTargetSeen > CharacterStats.LooseTargetTimeout)
	{
		UnlockCamera();
		return;
	}

	FRotator LookRotation = Controller->GetControlRotation();
	LookRotation.Yaw = Direction.Rotation().Yaw;

	if(InfomorphPC != nullptr)
	{
		float LastLookedTimer = InfomorphPC->GetLastLookedTimer();
		float LastMovedTimer = InfomorphPC->GetLastMovedTimer();
		if(LastLookedTimer > InfomorphPC->GetLookTimerTreshold())
		{
			if(LastMovedTimer > 0.5f)
			{
				Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(), LookRotation, DeltaSeconds, 8.0f));
			}
			else
			{
				Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(), LookRotation, DeltaSeconds, 10.0f));
			}

		}
	}
	else if(Controller != nullptr)
	{
		Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(), LookRotation, DeltaSeconds, 7.0f));
	}


	FRotator CharacterRotation = GetActorRotation();
	CharacterRotation.Yaw = CalculateTargetYaw(CharacterRotation, LookRotation, LockedCameraTimer);
	SetActorRotation(CharacterRotation);
}

void AInfomorphUE4Character::ProcessInteractionTarget(float DeltaSeconds)
{
	FVector Difference = InteractionTarget->GetComponentLocation() - GetActorLocation();
	Difference.Z = 0.0f;
	float DifferenceSize = Difference.Size();

	if(DifferenceSize < 30.0f)
	{
		if(InteractionRotateToTargetLerpTime == 0.0f)
		{
			AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
			if(InfomorphPC != nullptr)
			{
				InfomorphPC->InteractWithCurrentInteractable();
			}
			InteractionTarget = nullptr;
			
			return;
		}

		InteractionRotateToTargetTimer += DeltaSeconds;

		FRotator CharacterRotation = GetActorRotation();
		CharacterRotation.Yaw = CalculateTargetYaw(CharacterRotation, InteractionTarget->GetForwardVector().Rotation(), InteractionRotateToTargetTimer / InteractionRotateToTargetLerpTime);
		SetActorRotation(CharacterRotation);

		if(FMath::Abs(CharacterRotation.Yaw - InteractionTarget->GetForwardVector().Rotation().Yaw) < 3.0f)
		{
			AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
			if(InfomorphPC != nullptr)
			{
				InfomorphPC->InteractWithCurrentInteractable();
			}
			InteractionTarget = nullptr;
		}

		return;
	}

	InteractionTargetSetTimer += DeltaSeconds;

	if(InteractionMoveToTargetLerpTime == 0.0f)
	{
		return;
	}

	FRotator CharacterRotation = GetActorRotation();
	CharacterRotation.Yaw = CalculateTargetYaw(CharacterRotation, Difference.Rotation(), InteractionTargetSetTimer / InteractionMoveToTargetLerpTime);
	SetActorRotation(CharacterRotation);

	FVector Direction = FRotationMatrix(FRotator(0.0f, CharacterRotation.Yaw, 0.0f)).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction);
}

void AInfomorphUE4Character::ProcessPossessionMaterial(float DeltaSeconds)
{
	float CurrentMultiplier = 0.0f;
	if(MaterialInstance != nullptr)
	{
		MaterialInstance->GetScalarParameterValue("PossessedMultiplier", CurrentMultiplier);
	}

	if(Controller != nullptr && Controller->IsA<AInfomorphPlayerController>())
	{
		CurrentMultiplier += DeltaSeconds;
	}
	else
	{
		CurrentMultiplier -= DeltaSeconds;
	}

	if(MaterialInstance != nullptr)
	{
		MaterialInstance->SetScalarParameterValue("PossessedMultiplier", FMath::Clamp(CurrentMultiplier, 0.0f, 1.0f));
	}
}

void AInfomorphUE4Character::ProcessCombatMode(float DeltaSeconds)
{
	CombatModeCheckTimer += DeltaSeconds;
	if(CombatModeCheckTimer >= 1.0f)
	{
		CombatModeCheckTimer = 0.0f;
		bool bWasInCombat = bIsInCombatMode;
		CheckIfInCombatMode();
		if(bWasInCombat ^ bIsInCombatMode)
		{
			if(bIsInCombatMode)
			{
				int32 CombatSoundIndex = FMath::RandRange(1, CombatSounds.Num()) - 1;
				if(CombatSoundIndex < CombatSounds.Num())
				{
					CombatAudioComponent->SetSound(CombatSounds[CombatSoundIndex]);
					CombatAudioComponent->SetVolumeMultiplier(0.0f);
					CombatAudioComponent->Play();
				}
			}
			else
			{
				AudioComponent->SetVolumeMultiplier(0.0f);
				AudioComponent->Play();
			}
		}
	}

	if(bIsInCombatMode)
	{
		float CombatVolume = CombatAudioComponent->VolumeMultiplier;
		CombatVolume += 3.0f * DeltaSeconds;
		CombatVolume = FMath::Clamp(CombatVolume, 0.0f, 1.0f);
		if(CombatVolume >= 1.0f)
		{
			AudioComponent->Stop();
		}
		CombatAudioComponent->SetVolumeMultiplier(CombatVolume);
		AudioComponent->SetVolumeMultiplier(1.0f - CombatVolume);
	}
	else
	{
		float AmbientVolume = AudioComponent->VolumeMultiplier;
		if(bIsChangingAmbient)
		{
			AmbientVolume -= 3.0f * DeltaSeconds;
			if(AmbientVolume <= 0.0f)
			{
				bAmbientChanged = true;
				bIsChangingAmbient = false;
				AudioComponent->SetSound(CurrentAmbientSound);
			}
		}
		else if(bAmbientChanged)
		{
			AmbientVolume += 3.0f * DeltaSeconds;
			if(AmbientVolume >= 1.0f)
			{
				bAmbientChanged = false;
			}
		}
		else
		{
			AmbientVolume += 3.0f * DeltaSeconds;
			if(AmbientVolume >= 1.0f)
			{
				CombatAudioComponent->Stop();
			}
			CombatAudioComponent->SetVolumeMultiplier(1.0f - FMath::Clamp(AmbientVolume, 0.0f, 1.0f));
		}
		AudioComponent->SetVolumeMultiplier(FMath::Clamp(AmbientVolume, 0.0f, 1.0f));
	}
}

void AInfomorphUE4Character::ProcessConsciousnessRegeneration(float DeltaSeconds)
{
	if(GetWorld() == nullptr)
	{
		return;
	}

	float RealTimeSeconds = GetWorld()->GetRealTimeSeconds();
	if(bIsInCombatMode)
	{
		LastCombatModeTime = RealTimeSeconds;
	}
	else
	{
		if(RealTimeSeconds - LastCombatModeTime >= CharacterStats.ConsciousnessRegenerationCooldown)
		{
			CharacterStats.CurrentConsciousness = FMath::Clamp(CharacterStats.CurrentConsciousness + DeltaSeconds * CharacterStats.ConsciousnessRecoveryPerSecond, 0.0f, CharacterStats.BaseConsciousness);
		}
	}
}

void AInfomorphUE4Character::CheckIfInCombatMode()
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	static const FName TraceTag = TEXT("CombatModeCheckTrace");

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams TraceParams(TraceTag, true, this);
	FCollisionShape CollisionShape;
	CollisionShape.SetBox(FVector(CharacterStats.SightRange * 0.5f, CharacterStats.SightRange * 0.5f, CharacterStats.SightRange * 0.1f));

	bIsInCombatMode = false;
	TArray<FHitResult> Hits;
	bool bWasHit = World->SweepMultiByObjectType(Hits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape, TraceParams);
	if(bWasHit)
	{
		const int32 HitsCount = Hits.Num();
		for(int32 i = 0; i < HitsCount; ++i)
		{
			if(Hits[i].GetActor() != nullptr && Hits[i].GetActor()->IsA<AInfomorphUE4Character>())
			{
				bIsInCombatMode = IsTargetVisible(Hits[i].GetActor()) || (GetCameraTarget() != nullptr && GetCameraTarget()->IsA<AInfomorphUE4Character>());
				if(bIsInCombatMode)
				{
					return;
				}
			}
		}
	}
}

void AInfomorphUE4Character::ProcessFalling(float DeltaSeconds)
{
	float VelocityZ = GetCharacterMovement()->Velocity.Z;
	if(bIsFalling)
	{
		if(VelocityZ >= 0.0f)
		{
			if(IsFallingFromHigh())
			{
				float Damage = FallingTimer * 30.0f;
				if(FallingTimer >= 2.0f)
				{
					Damage = CharacterStats.BaseConsciousness;
				}
				CharacterStats.CurrentConsciousness = FMath::Clamp(CharacterStats.CurrentConsciousness - Damage, 0.0f, CharacterStats.BaseConsciousness);
			}

			bIsFalling = false;
			FallingTimer = 0.0f;
		}
	}
	else
	{
		if(VelocityZ < 0.0f)
		{
			bIsFalling = true;
		}
	}
	bIsInAir = VelocityZ != 0.0f;

	if(bIsFalling)
	{
		ResetBlocking();
		ResetDodging();
		ResetAttacks();
		FallingTimer += DeltaSeconds;
	}
	AInfomorphPlayerController* InfomorphPlayerController = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPlayerController != nullptr)
	{
		InfomorphPlayerController->SetMovementMultiplier(bIsFalling ? 0.0f : 1.0f);
	}
}

void AInfomorphUE4Character::DestroyActor()
{
	Destroy();
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->Destroy();
		CurrentSecondaryWeapon = nullptr;
	}
	if(CurrentShield != nullptr)
	{
		CurrentShield->Destroy();
		CurrentShield = nullptr;
	}
}

bool AInfomorphUE4Character::IsTargetVisible(AActor* Target) const
{
	UWorld* World = GetWorld();
	if(World == nullptr || Target == nullptr)
	{
		return false;
	}

	static const FName TraceTag = TEXT("TargetVisibleTest");

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	FCollisionQueryParams TraceParams(TraceTag, true, this);
	TraceParams.AddIgnoredActor(CurrentWeapon);
	TraceParams.AddIgnoredActor(CurrentSecondaryWeapon);
	TraceParams.AddIgnoredActor(CurrentShield);

	AActor* HitActor = nullptr;
	FHitResult Hit;
	bool bWasHit = World->LineTraceSingleByObjectType(Hit, GetEyesLocation(), Target->GetActorLocation(), ObjectQueryParams, TraceParams);

	if(bWasHit)
	{
		bWasHit = Hit.GetActor() != nullptr && Hit.GetActor()->WasRecentlyRendered();
	}

	return !bWasHit;
}

AInfomorphUE4Character::AInfomorphUE4Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 375.0f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	if(GetMesh() != nullptr)
		GetMesh()->bGenerateOverlapEvents = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->SetRelativeLocation(FVector(20.0f, 0.0f, 70.0f));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SoundComponent"));
	AudioComponent->SetupAttachment(FollowCamera);

	CombatAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CombatAudioComponent"));
	CombatAudioComponent->SetupAttachment(FollowCamera);

	SFXAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SFXComponent"));
	SFXAudioComponent->SetupAttachment(FollowCamera);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(400.0f);
	InteractionSphere->bGenerateOverlapEvents = true;
	InteractionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(GetMesh(), "LightSocket");
	Light->CastShadows = 0;

	bIsInStealthMode = false;

	CameraTarget = nullptr;
	LockedCameraTimer = 0.0f;
	bIsCameraLocked = false;

	bIsLightAttack = false;
	bIsHeavyAttack = false;
	bIsSpecialAttack = false;
	bIsDodging = false;
	bWasHit = false;

	WeaponSocketName = "WeaponSocket";
	ShieldSocketName = "ShieldSocket";
}

void AInfomorphUE4Character::BeginPlay()
{
	MaterialInstance = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), GetMesh());

	if(MaterialInstance != nullptr)
	{
		MaterialInstance->SetScalarParameterValue("PossessedMultiplier", 0.0f);
		GetMesh()->SetMaterial(0, MaterialInstance);
	}

	CharacterStats.Initialize();
	LastTimeTargetSeen = -CharacterStats.LooseTargetTimeout;
	LastActionTime = -CharacterStats.EnergyRestoreCooldown;
	LastSpecialAttackTime = -CharacterStats.SpecialAttackCooldown;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);

	CurrentWeapon = GetWorld()->SpawnActor<AInfomorphWeapon>(WeaponClass.Get());
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->AttachToComponent((USceneComponent*)GetMesh(), AttachmentRules, WeaponSocketName);
		CurrentWeapon->SetActorRelativeLocation(FVector::ZeroVector);
		CurrentWeapon->SetActorRelativeRotation(FQuat::Identity);
	}
	CurrentSecondaryWeapon = GetWorld()->SpawnActor<AInfomorphWeapon>(SecondaryWeaponClass.Get());
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->AttachToComponent((USceneComponent*)GetMesh(), AttachmentRules, SecondaryWeaponSocketName);
		CurrentSecondaryWeapon->SetActorRelativeLocation(FVector::ZeroVector);
		CurrentSecondaryWeapon->SetActorRelativeRotation(FQuat::Identity);
	}

	CurrentShield = GetWorld()->SpawnActor<AInfomorphShield>(ShieldClass.Get());
	if(CurrentShield != nullptr)
	{
		CurrentShield->AttachToComponent((USceneComponent*)GetMesh(), AttachmentRules, ShieldSocketName);
		CurrentShield->SetActorRelativeLocation(FVector::ZeroVector);
		CurrentShield->SetActorRelativeRotation(FQuat::Identity);
	}

	ResetState();

	InitialLocation = GetActorLocation();

	MovementState = EMovementState::Normal;

	Super::BeginPlay();

	AudioComponent->SetSound(InitialAmbientSound);
	AudioComponent->Play();
	CurrentAmbientSound = InitialAmbientSound;
	CombatAudioComponent->Stop();
}

void AInfomorphUE4Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetWorld()->GetRealTimeSeconds() - LastActionTime > CharacterStats.EnergyRestoreCooldown)
	{
		float Multiplier = 1.0f;
		if(IsBlocking())
		{
			Multiplier = 0.5f;
		}

		CharacterStats.CurrentEnergy += CharacterStats.EnergyRecoveryPerSecond * DeltaSeconds * Multiplier;
		CharacterStats.CurrentEnergy = FMath::Clamp(CharacterStats.CurrentEnergy, 0.0f, CharacterStats.BaseEnergy);
	}

	ProcessPossessionMaterial(DeltaSeconds);

	if(InteractionTarget != nullptr)
	{
		ProcessInteractionTarget(DeltaSeconds);
	}

	if(bIsDodging)
	{
		//AddMovementInput(DodgeWorldDirection);
		FVector CurrentLocation = GetActorLocation();
		CurrentLocation += DodgeWorldDirection * CharacterStats.DodgeSpeed * DeltaSeconds;
		SetActorLocation(CurrentLocation, true);
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = !IsCameraLocked();

		float Speed = CharacterStats.MaxSpeed;
		if(MovementState == EMovementState::Patrol)
		{
			Speed = CharacterStats.PatrolSpeed;
		}
		GetCharacterMovement()->MaxWalkSpeed = Speed * (IsInStealthMode() || IsBlocking() ? 0.5f : 1.0f);
	}

	if(PrepareAttackTime > 0.0f)
	{
		PrepareAttackTime -= DeltaSeconds;
		AddMovementInput(BeforeAttackDirection, PrepareAttackTime * 2.0f);
	}

	if(bIsCameraLocked && CameraTarget != nullptr)
	{
		ProcessCameraLocked(DeltaSeconds);
	}

	ProcessFalling(DeltaSeconds);

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPC == nullptr)
	{
		//Avoid combat checking and audio changing on AI characters
		CombatAudioComponent->Stop();
		AudioComponent->Stop();
		return;
	}

	ProcessCombatMode(DeltaSeconds);
	ProcessConsciousnessRegeneration(DeltaSeconds);
}

void AInfomorphUE4Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(NewController == nullptr)
	{
		return;
	}

	ResetState();
	MovementState = EMovementState::Normal;

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(NewController);
	if(InfomorphPC != nullptr)
	{
		CheckIfInCombatMode();
		if(bIsInCombatMode)
		{
			CombatAudioComponent->SetVolumeMultiplier(1.0f);
			CombatAudioComponent->Play();
		}
		else
		{
			AudioComponent->SetVolumeMultiplier(1.0f);
			AudioComponent->Play();
		}
		bIsInCombatMode = false;

		InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		InteractionSphere->bGenerateOverlapEvents = true;
		Light->SetVisibility(true);
	}
	else
	{
		//Check if this is an AI controller
		CombatAudioComponent->Stop();
		AudioComponent->Stop();
		AInfomorphBaseAIController* AIController = Cast<AInfomorphBaseAIController>(NewController);
		if(AIController != nullptr)
		{
			InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			InteractionSphere->bGenerateOverlapEvents = false;
			Light->SetVisibility(false);
		}
	}
}

float AInfomorphUE4Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(IsDead())
	{
		return 0.0f;
	}

	if(IsDodging() || IsDodgingZeroInput())
	{
		return 0.0f;
	}

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());

	if (InfomorphPC != nullptr && InfomorphPC->GetIsGodMode())
	{
		return 0.0f;
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	float EnergyLost = ActualDamage * 0.25f;
	AInfomorphUE4Character* Predator = Cast<AInfomorphUE4Character>(DamageCauser);
	float Dot = 0.0f;
	if(Predator != nullptr)
	{
		FVector PredatorForward = Predator->GetActorForwardVector();
		FVector Forward = GetActorForwardVector();
		Dot = FVector::DotProduct(PredatorForward, Forward);
	}
	bool bCanBlock = bIsBlocking && !IsAttacking() && !bShieldBroken && Dot < -0.5f;
	if(bCanBlock)
	{
		if(Predator != nullptr && Predator->CharacterStats.bBreaksBlockEveryHit)
		{
			EnergyLost = CharacterStats.CurrentEnergy + 10.0f;
		}
		else
		{
			EnergyLost = CharacterStats.BlockEnergyCost * ActualDamage;
		}
		ActualDamage = 0.0f;
		if(EnergyLost >= CharacterStats.CurrentEnergy)
		{
			EnergyLost = CharacterStats.CurrentEnergy;
			EndBlock();
			bShieldBroken = true;
			if(InfomorphPC != nullptr)
			{
				InfomorphPC->PlayFeedback(HitForceFeedback);
			}
		}
		else
		{
			if(Predator != nullptr)
			{
				Predator->bIsLightAttack = false;
				Predator->bIsHeavyAttack = false;
				Predator->bIsSpecialAttack = false;
				Predator->bWantsToLightAttack = false;
				Predator->bWantsToHeavyAttack = false;
				Predator->bWantsToSpecialAttack = false;
				Predator->bBlockHit = true;

				AInfomorphPlayerController* PlayerController = Cast<AInfomorphPlayerController>(Predator->GetController());
				if(PlayerController != nullptr)
				{
					PlayerController->SetMovementMultiplier(0.0f);
					PlayerController->PlayFeedback(HitForceFeedback);
				}
				else
				{
					AInfomorphBaseAIController* AIController = Cast<AInfomorphBaseAIController>(Predator->GetController());
					if(AIController != nullptr)
					{
						AIController->PauseBehaviorTree("BlockHit");
					}
				}
			}
			if(CurrentShield != nullptr)
			{
				CurrentShield->PlayHitSound();
			}
		}
	}

	if(InfomorphPC != nullptr)
	{
		ActualDamage *= (1.0f - CharacterStats.ConsciousnessArmorWhenPossessed);
	}

	CharacterStats.CurrentEnergy = FMath::Clamp(CharacterStats.CurrentEnergy - EnergyLost, 0.0f, CharacterStats.BaseEnergy);
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	CharacterStats.CurrentConsciousness = FMath::Clamp(CharacterStats.CurrentConsciousness - ActualDamage, 0.0f, CharacterStats.BaseConsciousness);
	bWasHit = ActualDamage > 0.0f;

	if(bWasHit)
	{
		if(InfomorphPC != nullptr)
		{
			InfomorphPC->PlayFeedback(HitForceFeedback);
			if(InfomorphPC->IsUsingSkill())
			{
				InfomorphPC->GetCurrentSkill()->StopUsing();
			}
		}
		
		if(IsConfused() || IsShieldBroken() || ActualDamage < CharacterStats.MinConsciousnessLostToPlayAnim)
		{
			bWasHit = false;
		}
		
		ResetAttacks();
		ResetDodging();
		AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
		if(InfomorphAIController != nullptr)
		{
			InfomorphAIController->PauseBehaviorTree("Hit");
		}

		if(IsDead())
		{
			Dedigitalize();
		}
		else
		{
			SFXAudioComponent->SetSound(HitSound);
			SFXAudioComponent->Play();
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
		}
	}

	return ActualDamage;
}

void AInfomorphUE4Character::FellOutOfWorld(const UDamageType& DamageType)
{
	if(!IsDead())
	{
		Dedigitalize();
	}
	CharacterStats.CurrentConsciousness = 0.0f;
}

void AInfomorphUE4Character::StartBlock()
{
	if(CharacterStats.bCanEverBlock)
	{
		bIsBlocking = true;
	}
}

void AInfomorphUE4Character::EndBlock()
{
	bIsBlocking = false;
}

void AInfomorphUE4Character::Dodge(const FVector& DodgeDirection)
{
	if(IsFalling())
	{
		return;
	}

	if(!CharacterStats.bCanEverDodge || CharacterStats.CurrentEnergy - CharacterStats.DodgeEnergyCost < 0.0f)
	{
		return;
	}

	if(IsBlocking())
	{
		EndBlock();
	}

	bIsLightAttack = bIsHeavyAttack = bIsSpecialAttack = bWantsToHeavyAttack = bWantsToLightAttack = bWantsToSpecialAttack = false;

	CharacterStats.CurrentEnergy -= CharacterStats.DodgeEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsDodging = true;

	if(DodgeDirection.Size() > 0.0f)
	{
		FRotator YawRotation(0.0f, GetEyesDirection().Rotation().Yaw, 0.0f);
		DodgeWorldDirection = YawRotation.RotateVector(DodgeDirection);
		DodgeWorldDirection.Z = 0.0f;
		DodgeWorldDirection.Normalize();
	}
	else
	{
		bIsDodgingZeroInput = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		DodgeWorldDirection = -GetActorForwardVector();
		DodgeWorldDirection.Z = 0.0f;
	}
	Crouch();
	DodgeWorldDirection.Normalize();
	//GetCharacterMovement()->MaxWalkSpeedCrouched = CharacterStats.DodgeSpeed;
}

void AInfomorphUE4Character::EnterStealthMode()
{
	if(bIsInStealthMode || !CharacterStats.bCanEverCrouch)
	{
		return;
	}

	Crouch();
	bIsInStealthMode = true;
}

void AInfomorphUE4Character::ExitStealthMode()
{
	if(!bIsInStealthMode)
	{
		return;
	}

	UnCrouch();
	bIsInStealthMode = false;
}

void AInfomorphUE4Character::Attack()
{
	if(IsAttacking())
	{
		if(bAttackQueueEnabled)
		{
			bWantsToLightAttack = !(bWantsToHeavyAttack || bWantsToSpecialAttack);
		}
		return;
	}

	if(CharacterStats.CurrentEnergy - CharacterStats.LightAttackEnergyCost < 0.0f)
	{
		EventNotEnoughEnergy();
		return;
	}

	CharacterStats.CurrentEnergy -= CharacterStats.LightAttackEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsLightAttack = true;
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->SetDamage(CharacterStats.LightAttackDamage);
	}
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->SetDamage(CharacterStats.LightAttackDamage);
	}
}

void AInfomorphUE4Character::HeavyAttack()
{
	if(IsAttacking())
	{
		if(bAttackQueueEnabled)
		{
			bWantsToHeavyAttack = !(bWantsToLightAttack || bWantsToSpecialAttack);
		}
		return;
	}

	if(CharacterStats.CurrentEnergy - CharacterStats.HeavyAttackEnergyCost < 0.0f)
	{
		EventNotEnoughEnergy();
		return;
	}

	CharacterStats.CurrentEnergy -= CharacterStats.HeavyAttackEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsHeavyAttack = true;
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->SetDamage(CharacterStats.HeavyAttackDamage);
	}
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->SetDamage(CharacterStats.HeavyAttackDamage);
	}
}

void AInfomorphUE4Character::SpecialAttack()
{
	if(IsAttacking())
	{
		if(bAttackQueueEnabled)
		{
			bWantsToSpecialAttack = !(bWantsToLightAttack || bWantsToHeavyAttack);
		}
		return;
	}

	if(CharacterStats.CurrentEnergy - CharacterStats.SpecialAttackEnergyCost < 0.0f)
	{
		EventNotEnoughEnergy();
		return;
	}
	if(GetWorld()->GetRealTimeSeconds() - LastSpecialAttackTime <= CharacterStats.SpecialAttackCooldown)
	{
		EventCooldownNotFinishedSpecial();
		return;
	}

	LastSpecialAttackTime = GetWorld()->GetRealTimeSeconds();

	CharacterStats.CurrentEnergy -= CharacterStats.SpecialAttackEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsSpecialAttack = true;
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->SetDamage(CharacterStats.SpecialAttackDamage);
	}
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->SetDamage(CharacterStats.SpecialAttackDamage);
	}
}

void AInfomorphUE4Character::SpecialAbility()
{

}

bool AInfomorphUE4Character::LockCameraOnTarget(AActor* Target)
{
	if(CameraTarget != nullptr && CameraTarget == Target)
	{
		return true;
	}

	UnlockCamera();

	if(Target == nullptr)
	{
		return false;
	}

	MovementState = EMovementState::TargetLocked;
	CameraTarget = Target;
	OnCameraLocked(CameraTarget);

	bIsCameraLocked = true;
	LockedCameraTimer = 0.0f;
	LastTimeTargetSeen = FPlatformTime::Seconds();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	return true;
}

void AInfomorphUE4Character::UnlockCamera()
{
	if(CameraTarget != nullptr)
	{
		OnCameraUnlocked(CameraTarget);
	}

	MovementState = EMovementState::Normal;
	bIsCameraLocked = false;
	CameraTarget = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AInfomorphUE4Character::Dedigitalize()
{
	ResetState();
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
	SFXAudioComponent->SetSound(DeathSound);
	SFXAudioComponent->Play();
	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPC == nullptr)
	{
		//It's AI so call Destroy after dedigitalize
		GetWorldTimerManager().SetTimer(DedigitalizeTimerHandle, this, &AInfomorphUE4Character::DestroyActor, 3.0f);
	}
	else
	{
		//It's player so respawn after dedigitialize
		GetWorldTimerManager().SetTimer(DedigitalizeTimerHandle, GetWorld()->GetAuthGameMode(), &AGameModeBase::ResetLevel, 3.0f);
	}
}

void AInfomorphUE4Character::SetNewAmbientSound(USoundBase* NewAmbientSound)
{
	if(CurrentAmbientSound == NewAmbientSound)
	{
		return;
	}
	CurrentAmbientSound = NewAmbientSound;
	bIsChangingAmbient = true;
	bAmbientChanged = false;
}

float AInfomorphUE4Character::GetPossessionChance(const FVector& PlayerLocation)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
	if(InfomorphAIController != nullptr && !InfomorphAIController->IsPlayerNoticed())
	{
		return 1.0f;
	}

	float ConsciousnessPercentage = CharacterStats.CurrentConsciousness / CharacterStats.BaseConsciousness;
	if(ConsciousnessPercentage <= CharacterStats.ConsciousnessPercentPossessable)
	{
		return 1.0f;
	}

	return 0.0f;
}

void AInfomorphUE4Character::PerformJump()
{
	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPC == nullptr)
	{
		return;
	}

	GetCharacterMovement()->Velocity = InfomorphPC->GetVelocityBeforeJump();
	Jump();
}

void AInfomorphUE4Character::Confuse(float ConfusionTime, float Multiplier)
{
	CharacterStats.bIsConfused = true;
	GetWorldTimerManager().SetTimer(ConfusionTimerHandle, this, &AInfomorphUE4Character::ConfusionEnd, ConfusionTime * Multiplier);

	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
	if(InfomorphAIController != nullptr)
	{
		InfomorphAIController->PauseBehaviorTree("Confusion");
	}
}

void AInfomorphUE4Character::ConfusionEnd()
{
	CharacterStats.bIsConfused = false;
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
	if(InfomorphAIController != nullptr)
	{
		InfomorphAIController->ResumeBehaviorTree();
	}
}

void AInfomorphUE4Character::SetInteractionTarget(USceneComponent* NewInteractionTarget)
{
	InteractionTarget = NewInteractionTarget;
	InteractionTargetSetTimer = 0.0f;
	InteractionRotateToTargetTimer = 0.0f;

	if(NewInteractionTarget != nullptr)
	{
		FVector Difference = NewInteractionTarget->GetComponentLocation() - GetActorLocation();
		Difference.Z = 0.0f;

		float TargetMoveYaw = CalculateTargetYaw(GetActorRotation(), Difference.Rotation(), 1.0f);
		float YawDifference = TargetMoveYaw - GetActorRotation().Yaw;
		InteractionMoveToTargetLerpTime = FMath::Abs(YawDifference) / 60.0f;

		float TargetRotateYaw = CalculateTargetYaw(FRotator(0.0f, TargetMoveYaw, 0.0f), NewInteractionTarget->GetForwardVector().Rotation(), 1.0f);
		YawDifference = TargetRotateYaw - (GetActorRotation().Yaw + YawDifference);
		InteractionRotateToTargetLerpTime = FMath::Abs(YawDifference) / 60.0f;
	}
}

float AInfomorphUE4Character::CalculateTargetYaw(const FRotator& CurrentRotation, const FRotator& TargetRotation, float LerpT) const
{
	float TargetYaw = TargetRotation.Yaw;
	if(FMath::Abs(TargetYaw - CurrentRotation.Yaw) > 180.0f)
	{
		TargetYaw = CurrentRotation.Yaw + FMath::Sign(CurrentRotation.Yaw) * (180.0f - FMath::Abs(CurrentRotation.Yaw) + 180.0f - FMath::Abs(TargetYaw));
	}
	return FMath::Lerp(CurrentRotation.Yaw, TargetYaw, FMath::Clamp(LerpT, 0.0f, 1.0f));
}

void AInfomorphUE4Character::OnCameraLocked_Implementation(AActor* NewTarget)
{
}

void AInfomorphUE4Character::OnCameraUnlocked_Implementation(AActor* OldTarget)
{
}

void AInfomorphUE4Character::EnableWeaponCollision()
{
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->EnableCollision();
	}
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->EnableCollision();
	}
}

void AInfomorphUE4Character::DisableWeaponCollision()
{
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->DisableCollision();
	}
	if(CurrentSecondaryWeapon != nullptr)
	{
		CurrentSecondaryWeapon->DisableCollision();
	}
}

FVector AInfomorphUE4Character::GetEyesLocation() const
{
	return FollowCamera != nullptr ? FollowCamera->GetComponentLocation() : GetActorLocation();
}

FVector AInfomorphUE4Character::GetEyesDirection() const
{
	return CameraBoom != nullptr ? CameraBoom->GetComponentRotation().Vector() : GetActorRotation().Vector();
}