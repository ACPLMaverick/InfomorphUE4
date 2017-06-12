// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphInteractable.h"
#include "InfomorphUE4.h"
#include "InfomorphUE4Character.h"


AInfomorphInteractable::AInfomorphInteractable() : Super()
{
	PrimaryActorTick.bCanEverTick = true; 
}

AInfomorphInteractable::AInfomorphInteractable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInfomorphInteractable::BeginPlay()
{
	Super::BeginPlay();
}

void AInfomorphInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AInfomorphInteractable::IsInteractionPossible() const
{
	return false;
}

void AInfomorphInteractable::Interact(AInfomorphUE4Character* Character)
{
	LogOnScreen("This should never appear");
}

void AInfomorphInteractable::StopInteracting()
{
	LogOnScreen("This should never appear");
}

USceneComponent* AInfomorphInteractable::GetInteractionTarget(AInfomorphUE4Character* Character) const
{
	return nullptr;
}
