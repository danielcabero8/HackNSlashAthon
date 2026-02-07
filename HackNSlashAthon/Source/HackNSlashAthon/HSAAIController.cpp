// Fill out your copyright notice in the Description page of Project Settings.

#include "HSAAIController.h"
#include "BehaviorTree/BlackboardData.h"

AHSAAIController::AHSAAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create runtime subobjects for behavior tree and blackboard components
	BrainComponent = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	Blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	// Perception setup
	PerceptionComp = ObjectInitializer.CreateDefaultSubobject<UAIPerceptionComponent>(this, TEXT("PerceptionComp"));
	SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));

	if (SightConfig)
	{
		// sensible defaults - tweak in editor or per-class defaults
		SightConfig->SightRadius = 2000.0f;
		SightConfig->LoseSightRadius = 2200.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->SetMaxAge(5.0f);

		// Detect all affiliations by default; change as needed
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	}

	if (PerceptionComp && SightConfig)
	{
		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AHSAAIController::OnTargetPerceptionUpdated);
	}

}

void AHSAAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Initialize blackboard and start the behavior tree if one is assigned
	if (BehaviorTreeAsset)
	{
		if (BehaviorTreeAsset->BlackboardAsset && Blackboard)
		{
			Blackboard->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset);
		}

		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AHSAAIController::OnUnPossess()
{
	// Stop the behavior tree when unpossessed
	if (BrainComponent != nullptr)
	{
		BrainComponent->StopLogic("Character Unpossessed");
	}

	Super::OnUnPossess();
}

void AHSAAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Blackboard)
	{
		return;
	}

	// TODO: this might be moved to the BT instead of controller
	//// If the stimulus reports the actor was successfully sensed -> set as target
	//if (Stimulus.WasSuccessfullySensed())
	//{
	//	BlackboardComp->SetValueAsObject(TargetActorBlackboardKey, Actor);
	//}
	//else
	//{
	//	// If the actor went out of sight and is the current target, clear it
	//	UObject* Current = BlackboardComp->GetValueAsObject(TargetActorBlackboardKey);
	//	if (Current == Actor)
	//	{
	//		BlackboardComp->SetValueAsObject(TargetActorBlackboardKey, nullptr);
	//	}
	//}
}