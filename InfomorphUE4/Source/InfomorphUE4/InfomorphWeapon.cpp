// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"
#include "Perception/AISense_Damage.h"

AInfomorphWeapon::AInfomorphWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->bGenerateOverlapEvents = true;
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AInfomorphWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->OnComponentBeginOverlap.AddDynamic(this, &AInfomorphWeapon::OnWeaponBeginOverlap);
	
	DisableCollision();
}

void AInfomorphWeapon::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OverlappedComponent != WeaponMesh)
	{
		return;
	}

	AInfomorphUE4Character* OtherCharacter = Cast<AInfomorphUE4Character>(OtherActor);
	if(OtherCharacter == nullptr)
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

void AInfomorphWeapon::EnableCollision()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->bGenerateOverlapEvents = true;
	IgnoredActors.Reset(IgnoredActors.Num());
}

void AInfomorphWeapon::DisableCollision()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->bGenerateOverlapEvents = false;
}

