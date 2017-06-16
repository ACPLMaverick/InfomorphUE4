// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphShield.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInfomorphShield::AInfomorphShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; 
	
	if(RootComponent == nullptr)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	}

	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(RootComponent);
	ShieldMesh->bGenerateOverlapEvents = false;
	ShieldMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ShieldMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void AInfomorphShield::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInfomorphShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInfomorphShield::PlayHitSound()
{
	UGameplayStatics::PlaySoundAtLocation((UObject*)GetWorld(), HitSound, GetActorLocation());
}

