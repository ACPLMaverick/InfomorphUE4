// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphInteractableDoors.h"
#include "InfomorphUE4.h"
#include "InfomorphUE4Character.h"
#include "InfomorphPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/ArrowComponent.h"

void AInfomorphInteractableDoors::OnNotCloseVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(bIsPlayerIn)
	{
		return;
	}

	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(OtherActor);
	if(Character == nullptr)
	{
		return;
	}

	if(OtherComp != (UPrimitiveComponent*)Character->GetCapsuleComponent())
	{
		return;
	}

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(Character->GetController());
	if(InfomorphPC != nullptr)
	{
		bIsPlayerIn = true;
	}
}

void AInfomorphInteractableDoors::OnNotCloseVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!bIsPlayerIn)
	{
		return;
	}

	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(OtherActor);
	if(Character == nullptr)
	{
		return;
	}

	if(OtherComp != (UPrimitiveComponent*)Character->GetCapsuleComponent())
	{
		return;
	}

	AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(Character->GetController());
	if(InfomorphPC != nullptr)
	{
		LastTimePlayerWasIn = GetWorld()->GetRealTimeSeconds();
		bIsPlayerIn = false;
	}
}

AInfomorphInteractableDoors::AInfomorphInteractableDoors(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	DoorsNotCloseVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("NotCloseVolume"));
	DoorsNotCloseVolume->SetBoxExtent(FVector(300.0f, 125.0f, 100.0f));
	DoorsNotCloseVolume->SetupAttachment(RootComponent);
	DoorsNotCloseVolume->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	DoorsNotCloseVolume->bGenerateOverlapEvents = true;
	DoorsNotCloseVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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
	AutoCloseCooldown = 3.0f;
}

void AInfomorphInteractableDoors::BeginPlay()
{
	Super::BeginPlay();

	bIsPlayerIn = false;
	bAreClosed = true;
	bAreClosing = false;
	bAreOpened = false;
	bAreOpening = false;

	DoorsNotCloseVolume->OnComponentBeginOverlap.AddDynamic(this, &AInfomorphInteractableDoors::OnNotCloseVolumeBeginOverlap);
	DoorsNotCloseVolume->OnComponentEndOverlap.AddDynamic(this, &AInfomorphInteractableDoors::OnNotCloseVolumeEndOverlap);
}

void AInfomorphInteractableDoors::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LogOnScreen(123, FColor::Magenta, FString::Printf(TEXT("bAreOpening: %s, bAreOpened: %s, bAreClosing: %s, bAreClosed: %s"), BOOL_TO_TCHAR(bAreOpening), BOOL_TO_TCHAR(bAreOpened), BOOL_TO_TCHAR(bAreClosing), BOOL_TO_TCHAR(bAreClosed)));
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

	if(bAreOpened)
	{
		if(!bIsPlayerIn && GetWorld()->GetRealTimeSeconds() - LastTimePlayerWasIn > AutoCloseCooldown)
		{
			bAreClosing = true;
			bAreOpened = false;
			ClosingTime = 0.0f;
		}
	}

	if(bAreClosing)
	{
		ClosingTime += DeltaSeconds * OpeningSpeed;

		if(ClosingTime >= 1.0f)
		{
			bAreClosed = true;
			bAreClosing = false;
		}

		ClosingTime = FMath::Clamp(ClosingTime, 0.0f, 1.0f);

		float LerpValue = OpeningCurve->GetFloatValue(1.0f - ClosingTime);
		LeftWings->SetWorldRotation(FMath::Lerp(LeftWingsClosedRotation, LeftWingsOpenedRotation, LerpValue));
		RightWings->SetWorldRotation(FMath::Lerp(RightWingsClosedRotation, RightWingsOpenedRotation, LerpValue));
	}
}

bool AInfomorphInteractableDoors::IsInteractionPossible() const
{
	return bAreClosed;
}

void AInfomorphInteractableDoors::Interact(AInfomorphUE4Character* Character)
{
	bAreOpened = false;
	bAreClosing = false;
	bAreClosed = false;
	bAreOpening = true;
	OpeningTime = 0.0f;
	ClosingTime = 0.0f;
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
	if(!bAreClosed)
	{
		bAreClosing = true;
	}
}

USceneComponent* AInfomorphInteractableDoors::GetInteractionTarget(AInfomorphUE4Character* Character) const
{
	float DistanceFromFront = FVector::Dist(Character->GetActorLocation(), FrontInteractionTarget->GetComponentLocation());
	float DistanceFromBack = FVector::Dist(Character->GetActorLocation(), BackInteractionTarget->GetComponentLocation());

	return DistanceFromBack < DistanceFromFront ? BackInteractionTarget : FrontInteractionTarget;
}
