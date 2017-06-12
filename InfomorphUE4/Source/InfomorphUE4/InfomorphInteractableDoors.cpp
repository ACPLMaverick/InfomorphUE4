// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphInteractableDoors.h"
#include "InfomorphUE4Character.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/ArrowComponent.h"

AInfomorphInteractableDoors::AInfomorphInteractableDoors(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	//DoorsOverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	//DoorsOverlapVolume->SetBoxExtent(FVector(300.0f, 125.0f, 100.0f));
	//DoorsOverlapVolume->SetupAttachment(RootComponent);
	//DoorsOverlapVolume->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	//DoorsOverlapVolume->bGenerateOverlapEvents = true;
	//DoorsOverlapVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	LeftWingsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("LeftWingsRoot"));
	RightWingsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RightWingsRoot"));
	LeftWings = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWings"));
	RightWings = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWings"));
	FrontInteractionTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontInteractionTarget"));
	BackInteractionTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("BackInteractionTarget"));

	LeftWingsRoot->SetupAttachment(RootComponent);
	RightWingsRoot->SetupAttachment(RootComponent);
	
	LeftWings->SetupAttachment(LeftWingsRoot);
	RightWings->SetupAttachment(RightWingsRoot);

	FrontInteractionTarget->SetupAttachment(RootComponent);
	BackInteractionTarget->SetupAttachment(RootComponent);

	OpeningSpeed = 1.0f;
	OpenAngle = 90.0f;
}

void AInfomorphInteractableDoors::Tick(float DeltaSeconds)
{
	if(bAreOpening)
	{
		OpeningTime += DeltaSeconds * OpeningSpeed;

		if(OpeningTime >= 1.0f)
		{
			bAreOpened = true;
			bAreOpening = false;
		}

		OpeningTime = FMath::Clamp(OpeningTime, 0.0f, 1.0f);

		float LerpValue = OpeningCurve->GetFloatValue(OpeningTime);
		LeftWings->SetWorldRotation(FMath::Lerp(LeftWingsClosedRotation, LeftWingsOpenedRotation, LerpValue));
		RightWings->SetWorldRotation(FMath::Lerp(RightWingsClosedRotation, RightWingsOpenedRotation, LerpValue));
	}
}

bool AInfomorphInteractableDoors::IsInteractionPossible() const
{
	return !bAreOpened && !bAreOpening;
}

void AInfomorphInteractableDoors::Interact(AInfomorphUE4Character* Character)
{
	OpeningTime = 0.0f;
	bAreOpening = true;
	LeftWingsClosedRotation = LeftWings->GetComponentRotation();
	RightWingsClosedRotation = RightWings->GetComponentRotation();

	float Dot = FVector::DotProduct(Character->GetActorForwardVector(), GetActorForwardVector());
	float Multiplier = FMath::Sign(Dot);

	LeftWingsOpenedRotation = LeftWingsClosedRotation + FRotator(0.0f, Multiplier * OpenAngle, 0.0f);
	RightWingsOpenedRotation = RightWingsClosedRotation + FRotator(0.0f, Multiplier * -OpenAngle, 0.0f);
}

void AInfomorphInteractableDoors::StopInteracting()
{
	bAreOpening = false;
	bAreOpened = false;
}

USceneComponent* AInfomorphInteractableDoors::GetInteractionTarget(AInfomorphUE4Character* Character) const
{
	float DistanceFromFront = FVector::Dist(Character->GetActorLocation(), FrontInteractionTarget->GetComponentLocation());
	float DistanceFromBack = FVector::Dist(Character->GetActorLocation(), BackInteractionTarget->GetComponentLocation());

	return DistanceFromBack < DistanceFromFront ? BackInteractionTarget : FrontInteractionTarget;
}
