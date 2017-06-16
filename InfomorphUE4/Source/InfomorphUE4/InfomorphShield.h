// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "InfomorphShield.generated.h"

UCLASS()
class INFOMORPHUE4_API AInfomorphShield : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Visual)
		class UStaticMeshComponent* ShieldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
		USoundBase* HitSound;
	
public:	
	// Sets default values for this actor's properties
	AInfomorphShield();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PlayHitSound();
	
};
