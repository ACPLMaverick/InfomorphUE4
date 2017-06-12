// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfomorphInteractable.generated.h"

UCLASS()
class INFOMORPHUE4_API AInfomorphInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfomorphInteractable();
	AInfomorphInteractable(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual bool IsInteractionPossible() const;
	virtual void Interact(class AInfomorphUE4Character* Character);
	virtual void StopInteracting();
	virtual USceneComponent* GetInteractionTarget(class AInfomorphUE4Character* Character) const;
};
