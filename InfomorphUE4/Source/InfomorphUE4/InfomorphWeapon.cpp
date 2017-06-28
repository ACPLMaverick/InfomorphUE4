// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "Perception/AISense_Damage.h"
#include "Components/CapsuleComponent.h"

AInfomorphWeapon::AInfomorphWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->bGenerateOverlapEvents = false;
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->SetupAttachment(WeaponMesh);
	CollisionCapsule->bGenerateOverlapEvents = true;
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AInfomorphWeapon::BeginPlay()
{
	Super::BeginPlay();

	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AInfomorphWeapon::OnWeaponBeginOverlap);

	DisableCollision();
}

void AInfomorphWeapon::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OverlappedComponent != CollisionCapsule)
	{
		return;
	}

	USceneComponent* ParentComponent = GetRootComponent()->GetAttachParent();
	if(ParentComponent == nullptr)
	{
		return;
	}

	AInfomorphUE4Character* ParentCharacter = Cast<AInfomorphUE4Character>(ParentComponent->GetOwner());
	if(ParentCharacter == nullptr || ParentCharacter == OtherActor)
	{
		return;
	}

	AInfomorphUE4Character* OtherCharacter = Cast<AInfomorphUE4Character>(OtherActor);
	if(OtherCharacter == nullptr)
	{
		return;
	}

	AController* Controller = ParentCharacter->GetController();
	AController* OtherController = OtherCharacter->GetController();
	if(Controller->IsA<AAIController>() && OtherController->IsA<AAIController>())
	{
		//Friendly fire off!
		return;
	}

	if(IgnoredActors.Contains(OtherCharacter))
	{
		//Prevent from double hitting!
		return;
	}

	OtherCharacter->TakeDamage(CurrentDamage, FDamageEvent(), ParentCharacter->GetController(), ParentCharacter);
	UAISense_Damage::ReportDamageEvent(GetWorld(), OtherCharacter, ParentCharacter, CurrentDamage, WeaponMesh->GetComponentLocation(), SweepResult.Location);
	//To prevent double hitting!
	IgnoredActors.Add(OtherCharacter);
}

void AInfomorphWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AInfomorphWeapon::IsCollisionEnabled() const
{
	if(CollisionCapsule == nullptr)
	{
		return false;
	}

	return CollisionCapsule->bGenerateOverlapEvents;
}

void AInfomorphWeapon::EnableCollision()
{
	CollisionCapsule->bGenerateOverlapEvents = true;
	IgnoredActors.Reset(IgnoredActors.Num());
}

void AInfomorphWeapon::DisableCollision()
{
	CollisionCapsule->bGenerateOverlapEvents = false;
}

