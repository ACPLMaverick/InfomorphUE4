// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphTutorialWidget.h"
#include "InfomorphUE4.h"
#include "Camera/CameraComponent.h"
#include "UObject/UObjectIterator.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

#include "DrawDebugHelpers.h"

void AInfomorphPlayerController::MoveForward(float Value)
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && Value != 0.0f && !PossessedCharacter->IsActionsDisabled())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//PossessedCharacter->AddMovementInput(Direction, Value);
		MoveInput += Direction * Value;
		LastMovedTimer = 0.0f;
	}
}

void AInfomorphPlayerController::MoveRight(float Value)
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && Value != 0.0f && !PossessedCharacter->IsActionsDisabled())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//PossessedCharacter->AddMovementInput(Direction, Value);
		MoveInput += Direction * Value;
		LastMovedTimer = 0.0f;
	}
}

void AInfomorphPlayerController::Turn(float Value)
{
	if(Value != 0.0f)
	{
		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && PossessedCharacter->IsCameraLocked())
		{
			return;
		}

		LastLookedTimer = 0.0f;
		AddYawInput(Value);
	}
}

void AInfomorphPlayerController::TurnAtRate(float Rate)
{
	if(Rate != 0.0f)
	{
		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && PossessedCharacter->IsCameraLocked())
		{
			return;
		}

		LastLookedTimer = 0.0f;
		AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AInfomorphPlayerController::LookUp(float Value)
{	
	if(Value != 0.0f)
	{
		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && PossessedCharacter->IsCameraLocked())
		{
			return;
		}

		LastLookedTimer = 0.0f;
		AddPitchInput(Value);
	}
}

void AInfomorphPlayerController::LookUpAtRate(float Rate)
{
	if(Rate != 0.0f)
	{
		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr && PossessedCharacter->IsCameraLocked())
		{
			return;
		}

		LastLookedTimer = 0.0f;
		AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
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
	if(PossessedCharacter != nullptr)
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
	if(CurrentTutorialWidget != nullptr)
	{
		CurrentTutorialWidget->CloseTutorial();
		return;
	}

	if(IsUsingSkill())
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsActionsDisabled() && !PossessedCharacter->IsInCombatMode())
	{
		if(IsInteractionPossible())
		{
			PossessedCharacter->UnlockCamera();
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
			TryLockCamera(nullptr);
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
		//PossessedCharacter->Jump();
		PossessedCharacter->SetWantsToJump(true);
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

void AInfomorphPlayerController::SetTutorialWidget(UInfomorphTutorialWidget* Widget)
{
	if (CurrentTutorialWidget != nullptr && Widget != nullptr)
	{
		CurrentTutorialWidget->CloseTutorial();
	}

	CurrentTutorialWidget = Widget;
	if(CurrentTutorialWidget != nullptr)
	{
		AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(PossessedCharacter != nullptr)
		{
			PossessedCharacter->ResetState();
		}
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
		float MinAngle = MaxAngle;
		float MaxAngleDeviation = 5.0f;
		float MaxScore = 0.0f;
		float ClosestDistance = MaxDistance;
		const int32 HitsSize = Hits.Num();
		for(int32 i = 0; i < HitsSize; ++i)
		{
			if(Hits[i].GetActor() == nullptr)
			{
				continue;
			}

			AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(Hits[i].GetActor());
			if(InfomorphCharacter == nullptr || InfomorphCharacter->IsDead())
			{
				continue;
			}

			FVector ToHitLocation = Hits[i].GetActor()->GetActorLocation() - EyesLocation;
			ToHitLocation.Normalize();
			float DotProduct = FVector::DotProduct(Direction, ToHitLocation);
			float Angle = FMath::Acos(DotProduct);

			if(Angle < MaxAngle)
			{
				float Distance = FVector::Distance(EyesLocation, Hits[i].GetActor()->GetActorLocation());
				if(Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
				}
				if(Angle < MinAngle)
				{
					MinAngle = Angle;
				}

				float AngleScore = DotProduct;
				float DistanceScore = 1.0f - FMath::Clamp((Distance - ClosestDistance) / MaxDistance, 0.0f, 1.0f);
				float Score = AngleScore * 0.8f + DistanceScore * 0.2f;
				if(Score > MaxScore)
				{
					MaxScore = Score;
					HitActor = Hits[i].GetActor();
				}
			}
		}
	}

	return HitActor;
}

AActor* AInfomorphPlayerController::GetNextActorInDirection(float MaxDistance, AActor* CurrentActor, const FVector& InputDirection)
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return nullptr;
	}
	APawn* ControlledPawn = GetPawn();
	if(ControlledPawn == nullptr)
	{
		return nullptr;
	}
	if(CurrentActor == nullptr)
	{
		return nullptr;
	}

	static const FName TraceTag = TEXT("LockTrace");

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams TraceParams(TraceTag, true, GetPawn());
	TraceParams.AddIgnoredActor(CurrentActor);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(MaxDistance);

	AActor* HitActor = nullptr;
	TArray<FHitResult> Hits;
	bool bWasHit = World->SweepMultiByObjectType(Hits, ControlledPawn->GetActorLocation(), ControlledPawn->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape, TraceParams);
	FVector2D Input2D(InputDirection.X, InputDirection.Y);

	if(bWasHit)
	{
		const int32 HitsCount = Hits.Num();
		float MaxScore = 0.0f;
		float ClosestDistance = MaxDistance;
		float ClosestFromCurrent = MaxDistance;

		TArray<AActor*> ActorsProcessed;
		for(int32 i = 0; i < HitsCount; ++i)
		{
			AActor* Candidate = Hits[i].GetActor();
			if(Candidate == nullptr || ActorsProcessed.Contains(Candidate))
			{
				continue;
			}
			ActorsProcessed.Add(Candidate);

			FVector CurrentToCandidate = Candidate->GetActorLocation() - CurrentActor->GetActorLocation();
			FVector2D CurrentToCandidate2D(CurrentToCandidate.X, CurrentToCandidate.Y);
			CurrentToCandidate2D.Normalize();
			float DotCandidate = FVector2D::DotProduct(CurrentToCandidate2D, Input2D);
			if(DotCandidate >= 0.0f)	//Is in the direction
			{
				float Distance = FVector::Distance(ControlledPawn->GetActorLocation(), Candidate->GetActorLocation());
				if(Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
				}
				float DistanceFromCurrent = FVector::Distance(CurrentActor->GetActorLocation(), Candidate->GetActorLocation());
				if(DistanceFromCurrent < ClosestFromCurrent)
				{
					ClosestFromCurrent = DistanceFromCurrent;
				}
			}
		}
		const int32 CandidatesCount = ActorsProcessed.Num();
		for(int32 i = 0; i < CandidatesCount; ++i)
		{
			AActor* Candidate = ActorsProcessed[i];
			float Distance = FVector::Distance(ControlledPawn->GetActorLocation(), Candidate->GetActorLocation());
			float DistanceFromCurrent = FVector::Distance(CurrentActor->GetActorLocation(), Candidate->GetActorLocation());
			
			float DistanceFromPlayerScore = 1.0f - FMath::Clamp((Distance - ClosestDistance) / MaxDistance, 0.0f, 1.0f);
			float DistanceFromCurrentScore = 1.0f - FMath::Clamp((DistanceFromCurrent - ClosestFromCurrent) / MaxDistance, 0.0f, 1.0f);
			float Score = 0.2f * DistanceFromCurrentScore + 0.8f * DistanceFromPlayerScore;

			if(Score >= MaxScore)
			{
				MaxScore = Score;
				HitActor = Candidate;
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
}

void AInfomorphPlayerController::TryLockCamera(AActor* CurrentTarget, const FVector& InputDirection)
{
	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead())
	{
		FVector EyesLocation = PossessedCharacter->GetEyesLocation();
		FVector Direction = PossessedCharacter->GetEyesDirection();
		float MaxDistance = PossessedCharacter->GetSightRange();
		if(CurrentTarget == nullptr)
		{
			AActor* PossibleTarget = GetActorInLookDirection(EyesLocation, Direction, MaxDistance);
			if(PossibleTarget != nullptr)
			{
				PossessedCharacter->LockCameraOnTarget(PossibleTarget);
			}
		}
		else
		{
			AActor* PossibleTarget = GetNextActorInDirection(MaxDistance, CurrentTarget, InputDirection);
			if(PossibleTarget != nullptr)
			{
				bRecentlyTriedToSwapCameraTarget = true;
				PossessedCharacter->LockCameraOnTarget(PossibleTarget);
			}
		}
	}
}

AInfomorphPlayerController::AInfomorphPlayerController() : Super()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	LookTimerThreshold = 0.75f;

	AutoReceiveInput = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;
}

AInfomorphPlayerController::AInfomorphPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	LookTimerThreshold = 0.75f;

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

	if(PossessedCharacter != nullptr && !PossessedCharacter->IsDead())
	{
		float Size = MoveInput.Size() * MovementMultiplier;
		if(Size > 0.0f)
		{
			MoveInput.Normalize();
			PossessedCharacter->AddMovementInput(MoveInput, FMath::RoundToInt(Size * 2.0f) * 0.5f * MovementMultiplier);

			MoveInput = FVector::ZeroVector;
		}

		LookForInteractables(DeltaSeconds);

		if(InputComponent != nullptr && PossessedCharacter->IsCameraLocked())
		{
			FVector InputDirection(-(InputComponent->GetAxisValue("LookUp") + InputComponent->GetAxisValue("LookUpRate")), InputComponent->GetAxisValue("Turn") + InputComponent->GetAxisValue("TurnRate"), 0.0f);
			InputDirection = GetControlRotation().RotateVector(InputDirection);
			if(bRecentlyTriedToSwapCameraTarget)
			{
				if(InputDirection.Size() < 0.3f)
				{
					bRecentlyTriedToSwapCameraTarget = false;
				}
			}
			else
			{
				if(InputDirection.Size() > 0.5f)
				{
					InputDirection.Normalize();
					TryLockCamera(PossessedCharacter->GetCameraTarget(), InputDirection);
				}
			}
		}
	}
	else
	{
		CurrentInteractable = nullptr;
	}

	if(IsUsingSkill())
	{
		Skills[CurrentSelectedSkillIndex].Skill->Tick(DeltaSeconds);
	}
}
