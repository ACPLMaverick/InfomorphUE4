// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphMovableDoors.h"
#include "InfomorphUE4.h"
#include "InfomorphUE4Character.h"
#include "InfomorphPlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Components/ArrowComponent.h"

void AInfomorphMovableDoors::OnNotCloseVolumeBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
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

void AInfomorphMovableDoors::OnNotCloseVolumeEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
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

AInfomorphMovableDoors::AInfomorphMovableDoors(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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

	DoorsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorsMesh"));
	DoorsMesh->SetupAttachment(RootComponent);

	FrontInteractionTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontInteractionTarget"));
	BackInteractionTarget = CreateDefaultSubobject<UArrowComponent>(TEXT("BackInteractionTarget"));
	MoveDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MoveDirection"));
	FrontInteractionTarget->SetupAttachment(RootComponent);
	BackInteractionTarget->SetupAttachment(RootComponent);
	MoveDirection->SetupAttachment(RootComponent);

	OpeningSpeed = 1.0f;
	OpenDistance = 100.0f;
	AutoCloseCooldown = 3.0f;
}

void AInfomorphMovableDoors::BeginPlay()
{
	Super::BeginPlay();

	bIsPlayerIn = false;
	bAreClosed = true;
	bAreClosing = false;
	bAreOpened = false;
	bAreOpening = false;

	DoorsNotCloseVolume->OnComponentBeginOverlap.AddDynamic(this, &AInfomorphMovableDoors::OnNotCloseVolumeBeginOverlap);
	DoorsNotCloseVolume->OnComponentEndOverlap.AddDynamic(this, &AInfomorphMovableDoors::OnNotCloseVolumeEndOverlap);
}

void AInfomorphMovableDoors::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
		DoorsMesh->SetWorldLocation(FMath::Lerp(DoorsClosedLocation, DoorsOpenedLocation, LerpValue));
	}

	if(bAreOpened)
	{
		if(!bIsPlayerIn && GetWorld()->GetRealTimeSeconds() - LastTimePlayerWasIn > AutoCloseCooldown)
		{
			bAreClosing = true;
			bAreOpened = false;
			ClosingTime = 0.0f;
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenCloseSound, GetActorLocation());
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
		DoorsMesh->SetWorldLocation(FMath::Lerp(DoorsClosedLocation, DoorsOpenedLocation, LerpValue));
	}
}

bool AInfomorphMovableDoors::IsInteractionPossible() const
{
	return bAreClosed;
}

void AInfomorphMovableDoors::Interact(AInfomorphUE4Character* Character)
{
	bAreOpened = false;
	bAreClosing = false;
	bAreClosed = false;
	bAreOpening = true;
	OpeningTime = 0.0f;
	ClosingTime = 0.0f;

	DoorsClosedLocation = DoorsMesh->GetComponentLocation();
	DoorsOpenedLocation = DoorsClosedLocation + MoveDirection->GetForwardVector() * OpenDistance;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), OpenCloseSound, GetActorLocation());
}

void AInfomorphMovableDoors::StopInteracting()
{
	bAreOpening = false;
	bAreOpened = false;
	if(!bAreClosed)
	{
		bAreClosing = true;
	}
}

USceneComponent* AInfomorphMovableDoors::GetInteractionTarget(AInfomorphUE4Character* Character) const
{
	float DistanceFromFront = FVector::Dist(Character->GetActorLocation(), FrontInteractionTarget->GetComponentLocation());
	float DistanceFromBack = FVector::Dist(Character->GetActorLocation(), BackInteractionTarget->GetComponentLocation());

	return DistanceFromBack < DistanceFromFront ? BackInteractionTarget : FrontInteractionTarget;
}
