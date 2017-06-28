// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "InfomorphTelekineticPawn.generated.h"

UCLASS()
class INFOMORPHUE4_API AInfomorphTelekineticPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Visual)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = Collision)
		USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere, Category = Collision)
		USphereComponent* AreaTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Telekinesis)
		FVector TargetLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Telekinesis)
		FRotator TargetRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Telekinesis)
		float MoveTime;

	FVector BaseLocation;
	FRotator BaseRotation;

	float Offset;
	float MovingTimer;
	bool bIsMoving;
	bool bIsMovingDown;

	bool bIsUsable;

	class AInfomorphPlayerController* InfomorphPC;

public:
	// Sets default values for this pawn's properties
	AInfomorphTelekineticPawn();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveToTarget(float HeightOffset, class AInfomorphPlayerController* InfomorphPlayerController);
	void Drop();

	void OnLeave(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = Telekinesis)
		bool IsUsable() const { return bIsUsable; }
};
