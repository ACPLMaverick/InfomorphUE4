// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphTutorialTrigger.h"
#include "InfomorphUE4Character.h"
#include "InfomorphPlayerController.h"
#include "InfomorphUE4.h"

#include "GameFramework/GameState.h"
#include "Components/BoxComponent.h"

// Sets default values
AInfomorphTutorialTrigger::AInfomorphTutorialTrigger()
{
	TutorialLauncherVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialLauncherVolume"));
	TutorialLauncherVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	TutorialLauncherVolume->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);

	RootComponent = TutorialLauncherVolume;

	PrimaryActorTick.bCanEverTick = true;
	bDestroyAfterShow = true;
}

// Called when the game starts or when spawned
void AInfomorphTutorialTrigger::BeginPlay()
{
	Super::BeginPlay();
	TutorialLauncherVolume->OnComponentBeginOverlap.AddDynamic(this, &AInfomorphTutorialTrigger::OnTutorialLauncherBeginOverlap);

	if (TutorialLifeTime > 0.0f)
	{
		bDestroyAfterShow = false;
	}
}

void AInfomorphTutorialTrigger::Tick(float DeltaTime)
{
	if (bAlreadyShown && TutorialLifeTime > 0.0f)
	{
		TutorialLifeTime -= DeltaTime;

		if (TutorialLifeTime <= 0.0f)
		{
			TutorialWidget->CloseTutorial();
			Destroy();
		}
	}
}

void AInfomorphTutorialTrigger::OnTutorialLauncherBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bAlreadyShown)
	{
		return;
	}

	AInfomorphUE4Character* Character = Cast<AInfomorphUE4Character>(OtherActor);
	if(Character == nullptr)
	{
		return;
	}

	AInfomorphPlayerController* PlayerController = Cast<AInfomorphPlayerController>(Character->GetController());
	if(PlayerController == nullptr)
	{
		return;
	}

	TutorialWidget = CreateWidget<UInfomorphTutorialWidget>(PlayerController, TutorialWidgetClass.Get());
	TutorialWidget->SetText(TutorialText);
	TutorialWidget->Show(PlayerController);
	bAlreadyShown = true;

	if(bDestroyAfterShow)
	{
		Destroy();
	}
}