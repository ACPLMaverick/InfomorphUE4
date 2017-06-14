// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "Camera/CameraComponent.h"
#include "UObject/UObjectIterator.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

void AInfomorphPlayerController::MoveForward(float Value)
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	float MovementValue = Value * MovementMultiplier;
	if(PossessedCharacter != nullptr && MovementValue != 0.0f && !PossessedCharacter->IsActionsDisabled())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		PossessedCharacter->AddMovementInput(Direction, MovementValue);
		LastMovedTimer = 0.0f;
	}
}

void AInfomorphPlayerController::MoveRight(float Value)
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	float MovementValue = Value * MovementMultiplier;
	if(PossessedCharacter != nullptr && MovementValue != 0.0f && !PossessedCharacter->IsActionsDisabled())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		PossessedCharacter->AddMovementInput(Direction, MovementValue);
		LastMovedTimer = 0.0f;
	}
}

void AInfomorphPlayerController::Turn(float Value)
{
	if(Value != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddYawInput(Value * LookMultiplier);

		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead() && PossessedCharacter->IsCameraLocked())
		{
			FVector EyesLocation = PossessedCharacter->GetEyesLocation();
			FVector Direction = PossessedCharacter->GetEyesDirection();
			float MaxDistance = PossessedCharacter->GetSightRange();
			AActor* PossibleTarget = GetActorInLookDirection(EyesLocation, Direction, MaxDistance);
			if(PossibleTarget != nullptr && PossibleTarget != PossessedCharacter->GetCameraTarget())
			{
				PossessedCharacter->LockCameraOnTarget(PossibleTarget);
			}
		}
	}
}

void AInfomorphPlayerController::TurnAtRate(float Rate)
{
	if(Rate != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * LookMultiplier);

		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead() && PossessedCharacter->IsCameraLocked())
		{
			FVector EyesLocation = PossessedCharacter->GetEyesLocation();
			FVector Direction = PossessedCharacter->GetEyesDirection();
			float MaxDistance = PossessedCharacter->GetSightRange();
			AActor* PossibleTarget = GetActorInLookDirection(EyesLocation, Direction, MaxDistance);
			if(PossibleTarget != nullptr && PossibleTarget != PossessedCharacter->GetCameraTarget())
			{
				PossessedCharacter->LockCameraOnTarget(PossibleTarget);
			}
		}
	}
}

void AInfomorphPlayerController::LookUp(float Value)
{
	if(Value != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddPitchInput(Value * LookMultiplier);

		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead() && PossessedCharacter->IsCameraLocked())
		{
			FVector EyesLocation = PossessedCharacter->GetEyesLocation();
			FVector Direction = PossessedCharacter->GetEyesDirection();
			float MaxDistance = PossessedCharacter->GetSightRange();
			AActor* PossibleTarget = GetActorInLookDirection(EyesLocation, Direction, MaxDistance);
			if(PossibleTarget != nullptr && PossibleTarget != PossessedCharacter->GetCameraTarget())
			{
				PossessedCharacter->LockCameraOnTarget(PossibleTarget);
			}
		}
	}
}

void AInfomorphPlayerController::LookUpAtRate(float Rate)
{
	if(Rate != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * LookMultiplier);

		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead() && PossessedCharacter->IsCameraLocked())
		{
			FVector EyesLocation = PossessedCharacter->GetEyesLocation();
			FVector Direction = PossessedCharacter->GetEyesDirection();
			float MaxDistance = PossessedCharacter->GetSightRange();
			AActor* PossibleTarget = GetActorInLookDirection(EyesLocation, Direction, MaxDistance);
			if(PossibleTarget != nullptr && PossibleTarget != PossessedCharacter->GetCameraTarget())
			{
				PossessedCharacter->LockCameraOnTarget(PossibleTarget);
			}
		}
	}
}

void AInfomorphPlayerController::PerformHeavyAttack()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->HeavyAttack();
	}
}

void AInfomorphPlayerController::PerformAttack()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->Attack();
	}
}

void AInfomorphPlayerController::PerformSpecialAttack()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->SpecialAttack();
	}
}

void AInfomorphPlayerController::PerformStartBlock()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead())
	{
		PossessedCharacter->StartBlock();
	}
}

void AInfomorphPlayerController::PerformEndBlock()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		PossessedCharacter->EndBlock();
	}
}

void AInfomorphPlayerController::PerformDodge()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		float Forward = InputComponent->GetAxisValue("MoveForward");
		float Right = InputComponent->GetAxisValue("MoveRight");
		FVector DodgeDirection(Forward, Right, 0.0f);
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->Dodge(DodgeDirection);
	}
}

void AInfomorphPlayerController::PerformStealthMode()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		else
		{
			PossessedCharacter->EnterStealthMode();
		}
	}
}

void AInfomorphPlayerController::PerformInteraction()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(IsInteractionPossible())
		{
			USceneComponent* InteractionTarget = CurrentInteractable->GetInteractionTarget(PossessedCharacter);
			PossessedCharacter->SetInteractionTarget(InteractionTarget);
			if(InteractionTarget == nullptr)
			{
				CurrentInteractable->Interact(PossessedCharacter);
			}
		}
	}
}

void AInfomorphPlayerController::PerformSpecialAbility()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		LogOnScreen("SpecialAbility");
	}
}

void AInfomorphPlayerController::PerformSpecialPossessedCharacterAbility()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		if(PossessedCharacter->IsBlocking())
		{
			PossessedCharacter->EndBlock();
		}
		PossessedCharacter->SpecialAbility();
	}
}

void AInfomorphPlayerController::PerformCameraLock()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		if(PossessedCharacter->IsCameraLocked())
		{
			PossessedCharacter->UnlockCamera();
		}
		else
		{
			FVector EyesLocation = PossessedCharacter->GetEyesLocation();
			FVector Direction = PossessedCharacter->GetEyesDirection();
			float MaxDistance = PossessedCharacter->GetSightRange();
			if(PossessedCharacter->LockCameraOnTarget(GetActorInLookDirection(EyesLocation, Direction, MaxDistance)))
			{
				LastLookedTimer = LookTimerThreshold - 0.02f;
			}
		}
	}
}

void AInfomorphPlayerController::PerformJump()
{
	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled())
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->Jump();
	}
}

void AInfomorphPlayerController::PerformStartSkillUsage()
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter->IsActionsDisabled())
	{
		return;
	}

	if(!Skills.IsValidIndex(CurrentSelectedSkillIndex) || Skills[CurrentSelectedSkillIndex].Skill == nullptr)
	{
		return;
	}

	Skills[CurrentSelectedSkillIndex].Skill->StartUsing(this);
}

void AInfomorphPlayerController::PerformStopSkillUsage()
{
	if(!Skills.IsValidIndex(CurrentSelectedSkillIndex) || Skills[CurrentSelectedSkillIndex].Skill == nullptr)
	{
		return;
	}

	Skills[CurrentSelectedSkillIndex].Skill->StopUsing();
}

void AInfomorphPlayerController::PerformSkillSelectUp()
{
	if(IsUsingSkill())
	{
		Skills[CurrentSelectedSkillIndex].Skill->StopUsing();
	}
	CurrentSelectedSkillIndex = (CurrentSelectedSkillIndex + 1) % Skills.Num();
}

void AInfomorphPlayerController::PerformSkillSelectDown()
{
	if(IsUsingSkill())
	{
		Skills[CurrentSelectedSkillIndex].Skill->StopUsing();
	}
	CurrentSelectedSkillIndex = (CurrentSelectedSkillIndex - 1) % Skills.Num();
}

void AInfomorphPlayerController::PossessNewCharacter(AInfomorphUE4Character* NewCharacter)
{
	AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(CurrentlyPossessedCharacter != nullptr)
	{
		CurrentlyPossessedCharacter->UnlockCamera();
	}
	Possess(NewCharacter);
	CurrentlyPossessedCharacter->SpawnDefaultController();
}

void AInfomorphPlayerController::MakeFootstepNoise()
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		float Speed = PossessedCharacter->GetCharacterMovement()->Velocity.Size();
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), PossessedCharacter->GetActorLocation(), 1.0f, PossessedCharacter, 1000.0f * Speed);
	}
}

void AInfomorphPlayerController::InteractWithCurrentInteractable()
{
	if(!IsInteractionPossible())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		CurrentInteractable->Interact(PossessedCharacter);
	}
}

AActor* AInfomorphPlayerController::GetActorInLookDirection(const FVector& EyesLocation, const FVector& Direction, float MaxDistance) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return nullptr;
	}

	static const FName TraceTag = TEXT("LockTrace");

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams TraceParams(TraceTag, true, GetPawn());
	//World->DebugDrawTraceTag = TraceTag;
	FCollisionShape CollisionShape;
	CollisionShape.SetBox(FVector(300.0f, 300.0f, 300.0f));
	FQuat Rotation = Direction.Rotation().Quaternion();

	AActor* HitActor = nullptr;
	TArray<FHitResult> Hits;
	bool bWasHit = World->SweepMultiByObjectType(Hits, EyesLocation, EyesLocation + Direction * MaxDistance, GetControlRotation().Quaternion(), ObjectQueryParams, CollisionShape, TraceParams);

	if(bWasHit)
	{
		float MaxAngle = 60.0f;
		float ClosestDistance = MaxDistance;
		const int32 HitsSize = Hits.Num();
		for(int32 i = 0; i < HitsSize; ++i)
		{
			if(Hits[i].GetActor() == nullptr || !Hits[i].GetActor()->IsA(AInfomorphUE4Character::StaticClass()))
			{
				continue;
			}

			FVector ToHitLocation = Hits[i].GetActor()->GetActorLocation() - EyesLocation;
			ToHitLocation.Normalize();
			float angle = FMath::Acos(FVector::DotProduct(Direction, ToHitLocation));
			if(angle < MaxAngle)
			{
				float Distance = FVector::Distance(EyesLocation, Hits[i].GetActor()->GetActorLocation());
				if(Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					HitActor = Hits[i].GetActor();
				}
			}
		}
	}

	return HitActor;
}

void AInfomorphPlayerController::LookForInteractables(float DeltaSeconds)
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter == nullptr)
	{
		return;
	}

	CurrentInteractable = nullptr;
	TArray<AActor*> InteractablesInRange;

	PossessedCharacter->GetInteractionSphere()->GetOverlappingActors(InteractablesInRange);

	int32 InteractablesNum = InteractablesInRange.Num();
	static const float MaxAngle = 60.0f;
	float MinAngle = MaxAngle;

	for(int32 i = 0; i < InteractablesNum; ++i)
	{
		AInfomorphInteractable* Interactable = Cast<AInfomorphInteractable>(InteractablesInRange[i]);
		if(Interactable == nullptr)
		{
			continue;
		}

		FVector CameraToInteractable = Interactable->GetActorLocation() - PossessedCharacter->GetEyesLocation();
		CameraToInteractable.Normalize();
		float Dot = FVector::DotProduct(CameraToInteractable, PossessedCharacter->GetEyesDirection());
		float Angle = FMath::Acos(Dot);
		if(Angle < MaxAngle && Angle < MinAngle)
		{
			MinAngle = Angle;
			CurrentInteractable = Interactable;
		}
	}

	if(CurrentInteractable != nullptr)
	{
		LogOnScreen(21, DeltaSeconds, FColor::Cyan, FString("CurrentInteractable: ").Append(CurrentInteractable->GetName()));
	}
}

AInfomorphPlayerController::AInfomorphPlayerController() : Super()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	LookTimerThreshold = 0.75f;

	MovementMultiplier = 1.0f;
	LookMultiplier = 1.0f;

	AutoReceiveInput = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;
}

AInfomorphPlayerController::AInfomorphPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	LookTimerThreshold = 0.75f;

	MovementMultiplier = 1.0f;
	LookMultiplier = 1.0f;

	AutoReceiveInput = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;

	InputComponent = CreateDefaultSubobject<UInputComponent>("InputComponent");
	FeedbackComponent = CreateDefaultSubobject<UForceFeedbackComponent>("FeedbackComponent");
}

void AInfomorphPlayerController::BeginPlay()
{
	CurrentSelectedSkillIndex = 0;
	int32 SkillsNum = Skills.Num();
	for(int32 i = 0; i < SkillsNum; ++i)
	{
		Skills[i].Initialize();
	}

	MovementMultiplier = 1.0f;
	LookMultiplier = 1.0f;
}

void AInfomorphPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	//Movement
	InputComponent->BindAxis("MoveForward", this, &AInfomorphPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AInfomorphPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AInfomorphPlayerController::Turn);
	InputComponent->BindAxis("TurnRate", this, &AInfomorphPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AInfomorphPlayerController::LookUp);
	InputComponent->BindAxis("LookUpRate", this, &AInfomorphPlayerController::LookUpAtRate);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformJump);
	InputComponent->BindAction("StealthMode", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformStealthMode);
	InputComponent->BindAction("CameraLock", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformCameraLock);

	//Attacks
	InputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformAttack);
	InputComponent->BindAction("HeavyAttack", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformHeavyAttack);
	InputComponent->BindAction("SpecialAttack", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformSpecialAttack);
	InputComponent->BindAction("SpecialAbility", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformSpecialAbility);
	InputComponent->BindAction("SpecialCharacterAbility", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformSpecialPossessedCharacterAbility);

	//Blocks and dodges
	InputComponent->BindAction("Block", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformStartBlock);
	InputComponent->BindAction("Block", EInputEvent::IE_Released, this, &AInfomorphPlayerController::PerformEndBlock);
	InputComponent->BindAction("Dodge", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformDodge);

	//Skills and interaction
	InputComponent->BindAction("SkillUse", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformStartSkillUsage);
	InputComponent->BindAction("SkillUse", EInputEvent::IE_Released, this, &AInfomorphPlayerController::PerformStopSkillUsage);
	InputComponent->BindAction("SkillSelectUp", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformSkillSelectUp);
	InputComponent->BindAction("SkillSelectDown", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformSkillSelectDown);
	InputComponent->BindAction("Interaction", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformInteraction);
}

void AInfomorphPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LastLookedTimer += DeltaSeconds;
	LastMovedTimer += DeltaSeconds;

	if(!Skills.IsValidIndex(CurrentSelectedSkillIndex) || Skills[CurrentSelectedSkillIndex].Skill == nullptr)
	{
		return;
	}
	
	FColor LogColor = FColor::Yellow;

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && PossessedCharacter->GetRatioRemainingToActivateSpecialAttack() > 0.0f)
	{
		LogColor = FColor(125, 125, 125, 255);
	}
	LogOnScreen(2, LogColor, FString::Printf(TEXT("Special attack, ratio: %.4f"), PossessedCharacter->GetRatioRemainingToActivateSpecialAttack()));

	LogColor = FColor::Yellow;
	if(!Skills[CurrentSelectedSkillIndex].Skill->CanBeUsed())
	{
		LogColor = FColor(125, 125, 125, 255);
	}

	if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead())
	{
		LookForInteractables(DeltaSeconds);
	}
	else
	{
		CurrentInteractable = nullptr;
	}

	LogOnScreen(1, LogColor, Skills[CurrentSelectedSkillIndex].SkillName.ToString().Append(FString::Printf(TEXT(", ratio: %.4f"), GetCurrentSkillRemainingRatio())));
	if(IsUsingSkill())
	{
		Skills[CurrentSelectedSkillIndex].Skill->Tick(DeltaSeconds);
	}
}
