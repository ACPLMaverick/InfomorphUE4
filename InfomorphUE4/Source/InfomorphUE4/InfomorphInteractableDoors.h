// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InfomorphInteractable.h"
#include "Components/BoxComponent.h"
#include "InfomorphInteractableDoors.generated.h"

/**
 * 
 */
UCLASS()
class INFOMORPHUE4_API AInfomorphInteractableDoors : public AInfomorphInteractable
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Doors)
		UBoxComponent* DoorsNotCloseVolume;
	UPROPERTY(EditAnywhere, Category = Doors)
		USceneComponent* LeftWingsRoot;
	UPROPERTY(EditAnywhere, Category = Doors)
		USceneComponent* RightWingsRoot;
	UPROPERTY(EditAnywhere, Category = Doors)
		UStaticMeshComponent* LeftWings;
	UPROPERTY(EditAnywhere, Category = Doors)
		UStaticMeshComponent* RightWings;
	UPROPERTY(EditAnywhere, Category = Doors)
		class UArrowComponent* FrontInteractionTarget;
	UPROPERTY(EditAnywhere, Category = Doors)
		class UArrowComponent* BackInteractionTarget;
	UPROPERTY(EditAnywhere, Category = Doors)
		class UCurveFloat* OpeningCurve;
	UPROPERTY(EditAnywhere, Category = Doors)
		class USoundBase* OpenCloseSound;
	UPROPERTY(EditAnywhere, Category = Doors)
		float OpeningSpeed;
	UPROPERTY(EditAnywhere, Category = Doors)
		float OpenAngle;
	UPROPERTY(EditAnywhere, Category = Doors)
		float AutoCloseCooldown;

	FRotator LeftWingsClosedRotation;
	FRotator RightWingsClosedRotation;
	FRotator LeftWingsOpenedRotation;
	FRotator RightWingsOpenedRotation;
	float OpeningTime;
	float ClosingTime;

	float LastTimePlayerWasIn;

	bool bAreOpening;
	bool bAreOpened;
	bool bAreClosing;
	bool bAreClosed;

	bool bIsPlayerIn;

protected:
	UFUNCTION()
		void OnNotCloseVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnNotCloseVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	AInfomorphInteractableDoors(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual bool IsInteractionPossible() const override;
	virtual void Interact(class AInfomorphUE4Character* Character) override;
	virtual void StopInteracting() override;
	virtual USceneComponent* GetInteractionTarget(class AInfomorphUE4Character* Character) const override;
};
