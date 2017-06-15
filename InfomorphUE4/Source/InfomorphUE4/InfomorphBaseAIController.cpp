// Fill out your copyright notice in the Description page of Project Settings.

#include "InfomorphBaseAIController.h"
#include "InfomorphUE4Character.h"
#include "InfomorphPlayerController.h"
#include "InfomorphUE4.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionSystem.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "DrawDebugHelpers.h"

void AInfomorphBaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if(Actor == nullptr)
	{
		return;
	}

	AInfomorphUE4Character* SensedCharacter = Cast<AInfomorphUE4Character>(Actor);
	if(SensedCharacter != nullptr)
	{
		AInfomorphPlayerController* InfomorphPC = Cast<AInfomorphPlayerController>(SensedCharacter->GetController());
		if(InfomorphPC != nullptr)
		{
			bIsPlayerNoticed = Stimulus.WasSuccessfullySensed();
			if(bIsPlayerNoticed)
			{
				Blackboard->SetValueAsObject("Target", SensedCharacter);
			}
			else
			{
				Blackboard->SetValueAsObject("Target", nullptr);
			}
		}
	}
}

AInfomorphBaseAIController::AInfomorphBaseAIController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bAttachToPawn = true;

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AInfomorphBaseAIController::OnTargetPerceptionUpdated);

	// sight sense
	SenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sense Config Sight"));
	SenseConfig_Sight->PeripheralVisionAngleDegrees = 30.0f;
	SenseConfig_Sight->SightRadius = 1000.0f;
	SenseConfig_Sight->LoseSightRadius = SenseConfig_Sight->SightRadius + 50.0f;
	SenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	SenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = true;
	SenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerception->ConfigureSense(*SenseConfig_Sight);

	// hearing sense
	SenseConfig_Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Sense Config Hearing"));
	SenseConfig_Hearing->HearingRange = 750.0f;
	SenseConfig_Hearing->LoSHearingRange = SenseConfig_Hearing->HearingRange + 50.0f;
	SenseConfig_Hearing->DetectionByAffiliation.bDetectEnemies = true;
	SenseConfig_Hearing->DetectionByAffiliation.bDetectFriendlies = true;
	SenseConfig_Hearing->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerception->ConfigureSense(*SenseConfig_Hearing);

	// damage sense
	SenseConfig_Damage = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Sense Config Damage"));
	AIPerception->ConfigureSense(*SenseConfig_Damage);

	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	AIPerception->SetDominantSense(SenseConfig_Sight->GetSenseImplementation());

	bIsPlayerNoticed = false;
}

void AInfomorphBaseAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	if(InPawn != nullptr && SenseConfig_Sight != nullptr)
	{
		UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, SenseConfig_Sight->GetSenseImplementation(), InPawn);
		UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, SenseConfig_Damage->GetSenseImplementation(), InPawn);
		UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, SenseConfig_Hearing->GetSenseImplementation(), InPawn);

		AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(InPawn);
		if(InfomorphCharacter != nullptr && InfomorphCharacter->BehaviorTree != nullptr)
		{
			SenseConfig_Hearing->HearingRange = InfomorphCharacter->GetCharacterStats().HearRange;
			SenseConfig_Hearing->LoSHearingRange = SenseConfig_Hearing->HearingRange * 1.5f;
			AIPerception->ConfigureSense(*SenseConfig_Hearing);

			if(InfomorphCharacter->BehaviorTree->BlackboardAsset != nullptr)
			{
				Blackboard->InitializeBlackboard(*(InfomorphCharacter->BehaviorTree->BlackboardAsset));
			}
			((UBehaviorTreeComponent*)BrainComponent)->StartTree(*(InfomorphCharacter->BehaviorTree));
		}
	}
}

void AInfomorphBaseAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AInfomorphUE4Character* InfomorphCharacter = Cast<AInfomorphUE4Character>(GetPawn());
	if(InfomorphCharacter != nullptr && InfomorphCharacter->IsDead())
	{
		BrainComponent->StopLogic("Death");
	}
}

void AInfomorphBaseAIController::PauseBehaviorTree(const FString& Reason)
{
	BrainComponent->StopLogic(Reason);
}

void AInfomorphBaseAIController::ResumeBehaviorTree()
{
	BrainComponent->RestartLogic();
}

void AInfomorphBaseAIController::DrawDebug(float DeltaSeconds)
{
	UWorld* world = GetWorld();
	APawn* pawn = GetPawn();
	if(world != nullptr && pawn != nullptr)
	{
		FTransform pawnTransform = pawn->GetTransform();
		FVector location = pawnTransform.GetLocation();

		if(SenseConfig_Sight != nullptr)
		{
			DrawDebugAltCone(world, pawnTransform.GetLocation(), FRotator(pawnTransform.GetRotation()), SenseConfig_Sight->SightRadius, SenseConfig_Sight->PeripheralVisionAngleDegrees, SenseConfig_Sight->PeripheralVisionAngleDegrees, FColor::Green.WithAlpha(50));
			DrawDebugAltCone(world, pawnTransform.GetLocation(), FRotator(pawnTransform.GetRotation()), SenseConfig_Sight->LoseSightRadius, SenseConfig_Sight->PeripheralVisionAngleDegrees, SenseConfig_Sight->PeripheralVisionAngleDegrees, FColor::Green.WithAlpha(15));
		}
		if(SenseConfig_Hearing != nullptr)
		{
			DrawDebugSphere(world, location, SenseConfig_Hearing->HearingRange, 16, FColor::Yellow.WithAlpha(50));
			DrawDebugSphere(world, location, SenseConfig_Hearing->LoSHearingRange, 16, FColor::Yellow.WithAlpha(15));
		}
	}
}