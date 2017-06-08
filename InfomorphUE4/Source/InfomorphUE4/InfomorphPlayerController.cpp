// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectIterator.h"

void AInfomorphPlayerController::MoveForward(float Value)
{
	if(bIsPossessing)
	{
		return;
	}

	ACharacter* PossessedCharacter = Cast<ACharacter>(GetPawn());
	if(PossessedCharacter != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		PossessedCharacter->AddMovementInput(Direction, Value);
		LastMovedTimer = 0.0f;
	}
}

void AInfomorphPlayerController::MoveRight(float Value)
{
	if(bIsPossessing)
	{
		return;
	}

	ACharacter* PossessedCharacter = Cast<ACharacter>(GetPawn());
	if(PossessedCharacter != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		PossessedCharacter->AddMovementInput(Direction, Value);
		LastMovedTimer = 0.0f;
	}
}

void AInfomorphPlayerController::Turn(float Value)
{
	if(bIsPossessing)
	{
		return;
	}

	if(Value != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddYawInput(Value);
	}
}

void AInfomorphPlayerController::TurnAtRate(float Rate)
{
	if(bIsPossessing)
	{
		return;
	}

	if(Rate != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AInfomorphPlayerController::LookUp(float Value)
{
	if(bIsPossessing)
	{
		return;
	}

	if(Value != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddPitchInput(Value);
	}
}

void AInfomorphPlayerController::LookUpAtRate(float Rate)
{
	if(bIsPossessing)
	{
		return;
	}

	if(Rate != 0.0f)
	{
		LastLookedTimer = 0.0f;
		AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AInfomorphPlayerController::PerformHeavyAttack()
{
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
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
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
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
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
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
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->StartBlock();
	}
}

void AInfomorphPlayerController::PerformEndBlock()
{
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->EndBlock();
	}
}

void AInfomorphPlayerController::PerformDodge()
{
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		PossessedCharacter->Dodge();
	}
}

void AInfomorphPlayerController::PerformStealthMode()
{
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
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
	if(bIsPossessing)
	{
		return;
	}

	LogOnScreen("Interaction");
}

void AInfomorphPlayerController::PerformSpecialAbility()
{
	if(bIsPossessing)
	{
		return;
	}

	LogOnScreen("SpecialAbility");
}

void AInfomorphPlayerController::PerformSpecialPossessedCharacterAbility()
{
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* PossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(PossessedCharacter != nullptr)
	{
		if(PossessedCharacter->IsInStealthMode())
		{
			PossessedCharacter->ExitStealthMode();
		}
		else
		{
			PossessedCharacter->SpecialAbility();
		}
	}
}

void AInfomorphPlayerController::PerformStartPossessing()
{
	if(bIsPossessing)
	{
		return;
	}

	AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(CurrentlyPossessedCharacter != nullptr)
	{
		if(CurrentlyPossessedCharacter->IsCameraLocked())
		{
			CharacterToPossess = Cast<AInfomorphUE4Character>(CurrentlyPossessedCharacter->GetCameraTarget());
			if(CharacterToPossess != nullptr)
			{
				PossessingTimer = 0.0f;
				bIsPossessing = true;
				SetViewTargetWithBlend(CharacterToPossess, PossessingTime, EViewTargetBlendFunction::VTBlend_Linear);
			}
		}
	}
}

void AInfomorphPlayerController::PerformStopPossessing()
{
	if(bIsPossessing)
	{
		AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
		if(CurrentlyPossessedCharacter != nullptr)
		{
			SetViewTargetWithBlend(CurrentlyPossessedCharacter, PossessingTime - PossessingTimer, EViewTargetBlendFunction::VTBlend_Linear);
		}
		CharacterToPossess = nullptr;
		bIsPossessing = false;
	}
}

void AInfomorphPlayerController::PerformCameraLock()
{
	if(bIsPossessing)
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
			if(PossessedCharacter->LockCameraOnTarget(GetActorInLookDirection(EyesLocation, Direction)))
			{
				LastLookedTimer = LookTimerThreshold - 0.02f;
			}
		}
	}
}

void AInfomorphPlayerController::PossessNewCharacter(AInfomorphUE4Character* NewCharacter)
{
	AInfomorphUE4Character* CurrentlyPossessedCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(CurrentlyPossessedCharacter != nullptr)
	{
		CurrentlyPossessedCharacter->UnlockCamera();
	}
	UnPossess();
	Possess(NewCharacter);
	bIsPossessing = false;
}

AActor* AInfomorphPlayerController::GetActorInLookDirection(const FVector& EyesLocation, const FVector& Direction) const
{
	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return nullptr;
	}

	static const FName TraceTag = TEXT("LockTrace");
	static const float MaxDistance = 3000.0f;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams TraceParams(TraceTag, true, GetPawn());
	World->DebugDrawTraceTag = TraceTag;
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

AInfomorphPlayerController::AInfomorphPlayerController() : Super()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	LookTimerThreshold = 0.75f;
	PossessingTime = 1.0f;
	PossessingTimer = 0.0f;
	bIsPossessing = false;

	AutoReceiveInput = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;
}

AInfomorphPlayerController::AInfomorphPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	LookTimerThreshold = 0.75f;
	PossessingTime = 1.0f;
	PossessingTimer = 0.0f;
	bIsPossessing = false;

	AutoReceiveInput = EAutoReceiveInput::Player0;
	PrimaryActorTick.bCanEverTick = true;

	InputComponent = CreateDefaultSubobject<UInputComponent>("InputComponent");
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

	//Possession and interaction
	InputComponent->BindAction("Possess", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformStartPossessing);
	InputComponent->BindAction("Possess", EInputEvent::IE_Released, this, &AInfomorphPlayerController::PerformStopPossessing);
	InputComponent->BindAction("Interaction", EInputEvent::IE_Pressed, this, &AInfomorphPlayerController::PerformInteraction);
}

void AInfomorphPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LastLookedTimer += DeltaSeconds;
	LastMovedTimer += DeltaSeconds;

	if(bIsPossessing)
	{
		PossessingTimer = FMath::Clamp(PossessingTimer + DeltaSeconds, 0.0f, PossessingTime);

		if(PossessingTimer >= PossessingTime)
		{
			PossessNewCharacter(CharacterToPossess);
		}
	}
}
