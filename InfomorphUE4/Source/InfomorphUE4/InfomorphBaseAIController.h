// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig.h"
#include "InfomorphBaseAIController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INFOMORPHUE4_API AInfomorphBaseAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception", meta = (BlueprintProtected = "true"))
		class UAIPerceptionComponent* AIPerception;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception", meta = (BlueprintProtected = "true"))
		class UAISenseConfig_Sight* SenseConfig_Sight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception", meta = (BlueprintProtected = "true"))
		class UAISenseConfig_Hearing* SenseConfig_Hearing;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception", meta = (BlueprintProtected = "true"))
		class UAISenseConfig_Damage* SenseConfig_Damage;

	float LastPlayerNoticedTime;
	bool bIsPlayerNoticed;

protected:
	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	void DrawDebug(float DeltaSeconds);

public:
	AInfomorphBaseAIController(const FObjectInitializer& ObjectInitializer);

	virtual void Possess(APawn* InPawn) override;

	virtual void Tick(float DeltaSeconds) override;

	void PauseBehaviorTree(const FString& Reason);
	void ResumeBehaviorTree();

	bool StartBlock();
	bool EndBlock();

	bool PerformAttack();
	bool PerformHeavyAttack();
	bool PerformSpecialAttack();
	bool IsAttacking() const;

	UFUNCTION(BlueprintCallable, Category = Perception)
		bool IsPlayerNoticed() const 
	{
		return bIsPlayerNoticed; 
	}
};
