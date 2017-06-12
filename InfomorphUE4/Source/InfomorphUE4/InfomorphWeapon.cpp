// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "InfomorphUE4Character.h"
#include "InfomorphUE4.h"

// Sets default values
AInfomorphWeapon::AInfomorphWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->AttachTo(RootComponent);
	WeaponMesh->bGenerateOverlapEvents = true;
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
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

	OtherCharacter->TakeDamage(CurrentDamage, FDamageEvent(), ParentCharacter->GetController(), ParentCharacter);
}

// Called every frame
void AInfomorphWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInfomorphWeapon::EnableCollision()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AInfomorphWeapon::DisableCollision()
{
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

