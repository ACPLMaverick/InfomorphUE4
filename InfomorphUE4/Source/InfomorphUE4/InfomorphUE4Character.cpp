// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "InfomorphPlayerController.h"
#include "InfomorphBaseAIController.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"

FCharacterStats::FCharacterStats()
{
	BaseConsciousness = 80.0f;
	BaseEnergy = 100.0f;
	ConfusionPossessedTime = 2.0f;
	ConfusionUnpossessedTime = 3.0f;
	SightRange = 1000.0f;
	HearRange = 750.0f;
	LooseTargetTimeout = 2.0f;
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
	LockedCameraTimer = FMath::Clamp(LockedCameraTimer + DeltaSeconds, 0.0f, 1.0f);

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
	float TargetYaw = LookRotation.Yaw;
	if(FMath::Abs(LookRotation.Yaw - CharacterRotation.Yaw) > 180.0f)
	{
		TargetYaw = CharacterRotation.Yaw + FMath::Sign(CharacterRotation.Yaw) * (180.0f - FMath::Abs(CharacterRotation.Yaw) + 180.0f - FMath::Abs(LookRotation.Yaw));
	}
	CharacterRotation.Yaw = FMath::Lerp(CharacterRotation.Yaw, TargetYaw, LockedCameraTimer);
	SetActorRotation(CharacterRotation);
}

void AInfomorphUE4Character::ConfusionEnd()
{
	CharacterStats.bIsConfused = false;
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
	GetCharacterMovement()->JumpZVelocity = 600.f;
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

	bIsInStealthMode = false;

	CameraTarget = nullptr;
	LockedCameraTimer = 0.0f;
	bIsCameraLocked = false;
}

void AInfomorphUE4Character::BeginPlay()
{
	Super::BeginPlay();

	CharacterStats.Initialize();
}

void AInfomorphUE4Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
	}
	else
	{
		//Check if this is an AI controller
		AInfomorphBaseAIController* AIController = Cast<AInfomorphBaseAIController>(NewController);
		if(AIController != nullptr)
		{
			Confuse(CharacterStats.ConfusionUnpossessedTime);
		}
	}
}

void AInfomorphUE4Character::StartBlock()
{

}

void AInfomorphUE4Character::EndBlock()
{

}

void AInfomorphUE4Character::Dodge()
{

}

void AInfomorphUE4Character::EnterStealthMode()
{
	if(bIsInStealthMode)
	{
		return;
	}

	Crouch();
	bIsInStealthMode = true;

	//TODO: Change movement speed
}

void AInfomorphUE4Character::ExitStealthMode()
{
	if(!bIsInStealthMode)
	{
		return;
	}

	UnCrouch();
	bIsInStealthMode = false;

	//TODO: Change movement speed
}

void AInfomorphUE4Character::Attack()
{
	LogOnScreen(FString("TEMPORARY ATTACK!"));
	
	AInfomorphUE4Character* Target = Cast<AInfomorphUE4Character>(CameraTarget);
	if(Target != nullptr)
	{
		Target->CharacterStats.CurrentConsciousness -= 15.0f;
	}
}

void AInfomorphUE4Character::HeavyAttack()
{

}

void AInfomorphUE4Character::SpecialAttack()
{

}

void AInfomorphUE4Character::SpecialAbility()
{

}

bool AInfomorphUE4Character::LockCameraOnTarget(AActor* Target)
{
	CameraTarget = Target;
	if(CameraTarget == nullptr)
	{
		UnlockCamera();
		return false;
	}

	bIsCameraLocked = true;
	LockedCameraTimer = 0.0f;
	LastTimeTargetSeen = FPlatformTime::Seconds();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	return true;
}

void AInfomorphUE4Character::UnlockCamera()
{
	bIsCameraLocked = false;
	CameraTarget = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
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

FVector AInfomorphUE4Character::GetEyesLocation() const
{
	return FollowCamera != nullptr ? FollowCamera->GetComponentLocation() : GetActorLocation();
}

FVector AInfomorphUE4Character::GetEyesDirection() const
{
	return FollowCamera != nullptr ? FollowCamera->GetComponentRotation().Vector() : GetActorRotation().Vector();
}