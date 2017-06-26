// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InfomorphInteractable.h"
#include "Components/BoxComponent.h"
#include "InfomorphInteractableElevator.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API AInfomorphInteractableElevator : public AInfomorphInteractable
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Elevator)
		UStaticMeshComponent* ElevatorMesh;
	UPROPERTY(EditAnywhere, Category = Elevator)
		UBoxComponent* ElevatorVolume;
	UPROPERTY(EditAnywhere, Category = Elevator)
		class UArrowComponent* InteractionTarget;
	UPROPERTY(EditAnywhere, Category = Elevator)
		class UCurveFloat* LocationCurve;
	UPROPERTY(EditAnywhere, Category = Elevator)
		FVector TargetOffsetFromBase;
	UPROPERTY(EditAnywhere, Category = Elevetor)
		float MoveTime;

	FVector BaseLocation;

	float MovingTimer;
	bool bIsMoving;
	bool bIsAtBasePosition;

public:
	AInfomorphInteractableElevator(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual bool IsInteractionPossible() const override;
	virtual void Interact(class AInfomorphUE4Character* Character) override;
	virtual void StopInteracting() override;
	virtual USceneComponent* GetInteractionTarget(class AInfomorphUE4Character* Character) const override;
};
