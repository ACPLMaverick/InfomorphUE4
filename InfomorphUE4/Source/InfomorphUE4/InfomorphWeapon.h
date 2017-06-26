// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "InfomorphWeapon.generated.h"

UCLASS()
class INFOMORPHUE4_API AInfomorphWeapon : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = Collision)
		class UCapsuleComponent* CollisionCapsule;
	UPROPERTY(EditAnywhere, Category = Visual)
		class UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sounds)
		USoundBase* AttackSound;
	
	float CurrentDamage;

	TArray<AActor*> IgnoredActors;

public:	
	// Sets default values for this actor's properties
	AInfomorphWeapon(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Damage)
		void SetDamage(float NewDamage)
	{
		CurrentDamage = NewDamage;
	}

	virtual void EnableCollision();
	virtual void DisableCollision();

	FORCEINLINE void PlayAttackSound()
	{
		UGameplayStatics::PlaySoundAtLocation((UObject*)GetWorld(), AttackSound, GetActorLocation());
	}
};
