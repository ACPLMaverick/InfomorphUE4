// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphTelekineticPawn.h"
#include "InfomorphPlayerController.h"
#include "InfomorphUE4Character.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"

// Sets default values
AInfomorphTelekineticPawn::AInfomorphTelekineticPawn() : Super()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	AreaTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("AreaTrigger"));
	if(RootComponent == nullptr)
	{
		RootComponent = (USceneComponent*)SphereCollision;
	}
	else
	{
		SphereCollision->SetupAttachment(RootComponent);
	}
	AreaTrigger->SetupAttachment(RootComponent);
	Mesh->SetupAttachment(SphereCollision);
	SphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	AreaTrigger->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	AreaTrigger->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	AreaTrigger->bGenerateOverlapEvents = true;
	AreaTrigger->OnComponentEndOverlap.AddDynamic(this, &AInfomorphTelekineticPawn::OnLeave);
	AreaTrigger->SetSphereRadius(512.0f);
	
	bIsUsable = true;
}

// Called every frame
void AInfomorphTelekineticPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> Overlapping;
	GetOverlappingActors(Overlapping, AInfomorphUE4Character::StaticClass());
	int32 OverlappingSize = Overlapping.Num();
	bool bPlayerFound = false;
	for (int32 i = 0; i < OverlappingSize; ++i)
	{
		AController* Controller = (Cast<AInfomorphUE4Character>(Overlapping[i]))->GetController();
		AInfomorphPlayerController* PC = Cast<AInfomorphPlayerController>(Controller);
		if (PC == nullptr)
		{
			continue;
		}

		PC->SetCurrentTelekineticPawn(this);
		bPlayerFound = true;

		break;
	}

	if (!bPlayerFound)
	{
		AInfomorphPlayerController* PC = Cast<AInfomorphPlayerController>(GetGameInstance()->GetFirstGamePlayer()->GetPlayerController(GetWorld()));
		PC->SetCurrentTelekineticPawn(nullptr);
	}

	if(bIsMoving)
	{
		MovingTimer += DeltaTime;
		float LerpT = MoveTime == 0.0f ? MovingTimer : MovingTimer / MoveTime;

		LerpT = FMath::Clamp(LerpT, 0.0f, 1.0f);

		SetActorLocation(FMath::Lerp(BaseLocation, TargetLocation + FVector::UpVector * Offset, LerpT));
		SetActorRotation(FQuat::Slerp(BaseRotation.Quaternion(), TargetRotation.Quaternion(), LerpT).Rotator());

		if(LerpT >= 1.0f)
		{
			bIsMovingDown = true;
			bIsMoving = false;
			MovingTimer = 0.0f;
		}
	}
	else if(bIsMovingDown)
	{
		MovingTimer += DeltaTime;

		float LerpT = FMath::Clamp(MovingTimer * 2.0f, 0.0f, 1.0f);
		SetActorLocation(TargetLocation + FVector::UpVector * FMath::Lerp(Offset, 0.0f, LerpT));

		if(MovingTimer >= 0.5f)
		{
			bIsMovingDown = false;
			if(InfomorphPC != nullptr)
			{
				AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(InfomorphPC->GetPawn());
				if(Character != nullptr && Character->GetCameraTarget() == this)
				{
					Character->UnlockCamera();
				}
			}
		}
	}
}

void AInfomorphTelekineticPawn::MoveToTarget(float HeightOffset, AInfomorphPlayerController* InfomorphPlayerController)
{
	InfomorphPC = InfomorphPlayerController;
	Offset = HeightOffset;

	BaseLocation = GetActorLocation();
	BaseRotation = GetActorRotation();

	MovingTimer = 0.0f;
	bIsMoving = true;
	bIsMovingDown = false;

	if(SphereCollision != nullptr)
	{
		SphereCollision->SetSimulatePhysics(false);
	}

	bIsUsable = false;
}

void AInfomorphTelekineticPawn::Drop()
{
	bIsMoving = false;

	if(SphereCollision != nullptr)
	{
		SphereCollision->SetSimulatePhysics(true);
	}
}

void AInfomorphTelekineticPawn::OnLeave(UPrimitiveComponent* OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AController* Controller = (Cast<AInfomorphUE4Character>(OtherActor))->GetController();
	AInfomorphPlayerController* PC = Cast<AInfomorphPlayerController>(Controller);
	if (PC != nullptr)
	{
		PC->SetCurrentTelekineticPawn(nullptr);
	}
}
