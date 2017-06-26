// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphInteractableElevator.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/ArrowComponent.h"
#include "InfomorphUE4.h"
#include "InfomorphUE4Character.h"
#include "InfomorphPlayerController.h"

AInfomorphInteractableElevator::AInfomorphInteractableElevator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	ElevatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ElevatorMesh"));
	ElevatorMesh->SetupAttachment(RootComponent);
	InteractionTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("InteractionTarget"));
	InteractionTarget->SetupAttachment(ElevatorMesh);
	ElevatorVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorVolume"));
	ElevatorVolume->SetupAttachment(ElevatorMesh);
	ElevatorVolume->bGenerateOverlapEvents = true;
	ElevatorVolume->SetCollisionProfileName("WorldStatic");
	ElevatorVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	MoveTime = 10.0f;
}

void AInfomorphInteractableElevator::BeginPlay()
{
	Super::BeginPlay();
	bIsMoving = false;
	bIsAtBasePosition = true;
	BaseLocation = GetActorLocation();
}

void AInfomorphInteractableElevator::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsMoving)
	{
		MovingTimer += DeltaSeconds;
		float Alpha = MoveTime != 0.0f ? MovingTimer / MoveTime : MovingTimer;

		float LerpAlpha = LocationCurve->GetFloatValue(FMath::Clamp(Alpha, 0.0f, 1.0f));
		if(!bIsAtBasePosition)
		{
			LerpAlpha = LocationCurve->GetFloatValue(FMath::Clamp(1.0f - Alpha, 0.0f, 1.0f));
		}
		FVector CurrentOffset = FMath::Lerp(FVector::ZeroVector, TargetOffsetFromBase, LerpAlpha);
		SetActorLocation(CurrentOffset + BaseLocation);

		if(Alpha >= 1.0f)
		{
			bIsAtBasePosition = !bIsAtBasePosition;
			bIsMoving = false;
		}
	}
}

bool AInfomorphInteractableElevator::IsInteractionPossible() const
{
	return !bIsMoving;
}

void AInfomorphInteractableElevator::Interact(AInfomorphUE4Character* Character)
{
	MovingTimer = 0.0f;
	bIsMoving = true;
}

void AInfomorphInteractableElevator::StopInteracting()
{
	bIsMoving = false;
}

USceneComponent* AInfomorphInteractableElevator::GetInteractionTarget(AInfomorphUE4Character* Character) const
{
	return (USceneComponent*)InteractionTarget;
}
