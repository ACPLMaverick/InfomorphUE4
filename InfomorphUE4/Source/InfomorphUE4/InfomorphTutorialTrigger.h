// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfomorphTutorialWidget.h"
#include "InfomorphTutorialTrigger.generated.h"

UCLASS()
class INFOMORPHUE4_API AInfomorphTutorialTrigger : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = Tutorial)
		class UBoxComponent* TutorialLauncherVolume;

	UPROPERTY(EditAnywhere, Category = Tutorial)
		TSubclassOf<UInfomorphTutorialWidget> TutorialWidgetClass;

	UPROPERTY(EditAnywhere, Category = Tutorial)
		bool bDestroyAfterShow;

	UInfomorphTutorialWidget* TutorialWidget;

public:	
	// Sets default values for this actor's properties
	AInfomorphTutorialTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnTutorialLauncherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
