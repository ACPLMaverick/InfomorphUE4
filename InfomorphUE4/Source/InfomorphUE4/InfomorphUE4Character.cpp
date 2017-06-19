// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "InfomorphPlayerController.h"
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
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

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
	if((TargetCharacter != nullptr && TargetCharacter->IsDead()) || IsDead())
	{
		UnlockCamera();
		return;
	}

	FVector Direction = CameraTarget->GetActorLocation() - GetEyesLocation();
	
	if(Direction.Size() > CharacterStats.SightRange)
	{
		UnlockCamera();
		return;
	}

	Direction.Normalize();

	if(IsTargetVisible(Direction))
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

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPC != nullptr)
	{
		float LastLookedTimer = InfomorphPC->GetLastLookedTimer();
		float LastMovedTimer = InfomorphPC->GetLastMovedTimer();
		if(LastLookedTimer > InfomorphPC->GetLookTimerTreshold())
		{
			if(LastMovedTimer > 0.5f)
			{
				Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(), LookRotation, DeltaSeconds, 3.0f));
			}
			else
			{
				Controller->SetControlRotation(FMath::RInterpTo(Controller->GetControlRotation(), LookRotation, DeltaSeconds, 5.0f));
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

void AInfomorphUE4Character::ConfusionEnd()
{
	CharacterStats.bIsConfused = false;
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
	if(InfomorphAIController != nullptr)
	{
		InfomorphAIController->ResumeBehaviorTree();
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
	if(CurrentShield != nullptr)
	{
		CurrentShield->Destroy();
		CurrentShield = nullptr;
	}
}

bool AInfomorphUE4Character::IsTargetVisible(const FVector& Direction) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return false;
	}

	static const FName TraceTag = TEXT("TargetVisibleTest");

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams TraceParams(TraceTag, true, this);

	AActor* HitActor = nullptr;
	FHitResult Hit;
	bool bWasHit = World->LineTraceSingleByObjectType(Hit, GetEyesLocation(), GetEyesLocation() + Direction * CharacterStats.SightRange, ObjectQueryParams, TraceParams);

	return Hit.GetActor() != nullptr && Hit.GetActor()->IsA<AInfomorphUE4Character>();
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
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 375.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 100.0f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

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

	if(CurrentWeapon == nullptr)
	{
		CurrentWeapon = GetWorld()->SpawnActor<AInfomorphWeapon>(WeaponClass.Get());
		CurrentWeapon->AttachToComponent((USceneComponent*)GetMesh(), AttachmentRules, WeaponSocketName);
		CurrentWeapon->SetActorRelativeLocation(FVector::ZeroVector);
		CurrentWeapon->SetActorRelativeRotation(FQuat::Identity);
	}

	CurrentShield = GetWorld()->SpawnActor<AInfomorphShield>(ShieldClass.Get());
	CurrentShield->AttachToComponent((USceneComponent*)GetMesh(), AttachmentRules, ShieldSocketName);
	CurrentShield->SetActorRelativeLocation(FVector::ZeroVector);
	CurrentShield->SetActorRelativeRotation(FQuat::Identity);

	ResetState();

	InitialLocation = GetActorLocation();

	Super::BeginPlay();
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
		AddMovementInput(DodgeWorldDirection, CharacterStats.DodgeSpeed);
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = !IsCameraLocked();
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
}

void AInfomorphUE4Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(NewController == nullptr)
	{
		return;
	}

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(NewController);
	if(InfomorphPC != nullptr)
	{
		Confuse(CharacterStats.ConfusionPossessedTime);
		InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		InteractionSphere->bGenerateOverlapEvents = true;
		Light->SetVisibility(true);
	}
	else
	{
		//Check if this is an AI controller
		AInfomorphBaseAIController* AIController = Cast<AInfomorphBaseAIController>(NewController);
		if(AIController != nullptr)
		{
			Confuse(CharacterStats.ConfusionUnpossessedTime);
			InteractionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			InteractionSphere->bGenerateOverlapEvents = false;
			ResetState();
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

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	float EnergyLost = ActualDamage * 0.25f;
	if(bIsBlocking)
	{
		EnergyLost = CharacterStats.BlockEnergyCost * ActualDamage;
		ActualDamage = 0.0f;
		if(EnergyLost > CharacterStats.CurrentEnergy)
		{
			ActualDamage = EnergyLost - CharacterStats.CurrentEnergy;
			EnergyLost = CharacterStats.CurrentEnergy;
			EndBlock();
		}
		else
		{
			if(CurrentShield != nullptr)
			{
				CurrentShield->PlayHitSound();
			}
		}
	}

	if(GetController()->IsA<AInfomorphPlayerController>())
	{
		ActualDamage *= (1.0f - CharacterStats.ConsciousnessArmorWhenPossessed);
	}

	CharacterStats.CurrentEnergy = FMath::Clamp(CharacterStats.CurrentEnergy - EnergyLost, 0.0f, CharacterStats.BaseEnergy);
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	CharacterStats.CurrentConsciousness = FMath::Clamp(CharacterStats.CurrentConsciousness - ActualDamage, 0.0f, CharacterStats.BaseConsciousness);
	bWasHit = ActualDamage > 0.0f;

	if(bWasHit)
	{
		ResetAttacks();
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
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, GetActorLocation());
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
	bIsBlocking = true;
	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPC != nullptr)
	{
		InfomorphPC->SetMovementMultiplier(0.5f);
	}
}

void AInfomorphUE4Character::EndBlock()
{
	bIsBlocking = false;
	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(GetController());
	if(InfomorphPC != nullptr)
	{
		InfomorphPC->ResetMovementMultiplier();
	}
}

void AInfomorphUE4Character::Dodge(const FVector& DodgeDirection)
{
	if(CharacterStats.CurrentEnergy - CharacterStats.DodgeEnergyCost < 0.0f)
	{
		return;
	}

	if(IsBlocking())
	{
		EndBlock();
	}

	CharacterStats.CurrentEnergy -= CharacterStats.DodgeEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsDodging = true;

	if(DodgeDirection.Size() > 0.0f)
	{
		DodgeWorldDirection = GetEyesDirection().ToOrientationRotator().RotateVector(DodgeDirection);
	}
	else
	{
		bIsDodgingZeroInput = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		DodgeWorldDirection = -GetActorForwardVector();
		DodgeWorldDirection.Z = 0.0f;
	}
	DodgeWorldDirection.Normalize();
}

void AInfomorphUE4Character::EnterStealthMode()
{
	if(bIsInStealthMode)
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
	if(CharacterStats.CurrentEnergy - CharacterStats.LightAttackEnergyCost < 0.0f)
	{
		return;
	}

	CharacterStats.CurrentEnergy -= CharacterStats.LightAttackEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsLightAttack = true;
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->SetDamage(CharacterStats.LightAttackDamage);
	}
	BeforeAttackDirection = GetCharacterMovement()->Velocity;
	if(BeforeAttackDirection.Size() > 0.0f)
	{
		BeforeAttackDirection.Normalize();
		PrepareAttackTime = 0.5f;
	}
}

void AInfomorphUE4Character::HeavyAttack()
{
	if(CharacterStats.CurrentEnergy - CharacterStats.HeavyAttackEnergyCost < 0.0f)
	{
		return;
	}

	CharacterStats.CurrentEnergy -= CharacterStats.HeavyAttackEnergyCost;
	LastActionTime = GetWorld()->GetRealTimeSeconds();
	bIsHeavyAttack = true;
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->SetDamage(CharacterStats.HeavyAttackDamage);
	}
	BeforeAttackDirection = GetCharacterMovement()->Velocity;
	if(BeforeAttackDirection.Size() > 0.0f)
	{
		BeforeAttackDirection.Normalize();
		PrepareAttackTime = 0.5f;
	}
}

void AInfomorphUE4Character::SpecialAttack()
{
	if(CharacterStats.CurrentEnergy - CharacterStats.SpecialAttackEnergyCost < 0.0f)
	{
		return;
	}
	if(GetWorld()->GetRealTimeSeconds() - LastSpecialAttackTime <= CharacterStats.SpecialAttackCooldown)
	{
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
	BeforeAttackDirection = GetCharacterMovement()->Velocity;
	if(BeforeAttackDirection.Size() > 0.0f)
	{
		BeforeAttackDirection.Normalize();
		PrepareAttackTime = 0.5f;
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
	bIsCameraLocked = false;
	CameraTarget = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AInfomorphUE4Character::Dedigitalize()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
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

float AInfomorphUE4Character::GetPossessionChance(const FVector& PlayerLocation)
{
	AInfomorphBaseAIController* InfomorphAIController = Cast<AInfomorphBaseAIController>(GetController());
	if(InfomorphAIController != nullptr && !InfomorphAIController->IsPlayerNoticed())
	{
		return 1.0f;
	}

	float ConsciousnessPercentage = CharacterStats.CurrentConsciousness / CharacterStats.BaseConsciousness;
	if(ConsciousnessPercentage <= 0.25f)
	{
		return 1.0f;
	}

	FVector ActorForward = GetActorForwardVector();
	FVector ActorToPlayer = PlayerLocation - GetActorLocation();
	ActorToPlayer.Normalize();
	float DotProduct = FVector::DotProduct(ActorForward, ActorToPlayer);
	if(DotProduct > 0.5f)
	{
		return 0.0f;
	}

	float PercentageInversed = 1.0f - ConsciousnessPercentage;
	float Chance = FMath::Clamp((-2.0f * DotProduct + 1.0f) / 3.0f, 0.0f, 1.0f);
	return Chance * PercentageInversed;
}

void AInfomorphUE4Character::Confuse(float ConfusionTime, float Multiplier)
{
	CharacterStats.bIsConfused = true;
	GetWorldTimerManager().SetTimer(ConfusionTimerHandle, this, &AInfomorphUE4Character::ConfusionEnd, ConfusionTime * Multiplier);
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
}

void AInfomorphUE4Character::DisableWeaponCollision()
{
	if(CurrentWeapon != nullptr)
	{
		CurrentWeapon->DisableCollision();
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