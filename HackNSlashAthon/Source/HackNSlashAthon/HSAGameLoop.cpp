#include "HSAGameLoop.h"

#include "HSAGameInstance.h"
#include "HSALevelGeneration.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void UHSAGameLoop::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UHSALevelGeneration>();
	Super::Initialize(Collection);

	if (UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem())
	{
		LevelGen->OnLevelGenerated.AddDynamic(this, &UHSAGameLoop::OnLevelGenerated);
	}
}

void UHSAGameLoop::Tick(float DeltaTime)
{
	// Your logic here!
	// Example: Update your MapGridRow logic or timers
}

bool UHSAGameLoop::IsTickable() const
{
	// Only tick if we are not a template (CDO) and the subsystem is initialized
	return !IsTemplate();
}

TStatId UHSAGameLoop::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyTickableSubsystem, STATGROUP_Tickables);
}

void UHSAGameLoop::SetState(EHSAGameState state)
{
	CurrentState = state;

	switch (CurrentState)
	{
	case EHSAGameState::LoadingLevel:
		OnLoadingLevel();
		break;
	case EHSAGameState::TransitioningToLevel:
		OnTransitioningToLevel();
		break;
	case EHSAGameState::PlayingLevel:
		OnPlayingLevel();
		break;
	case EHSAGameState::TransitioningOut:
		OnTransitioningOut();
		break;
	case EHSAGameState::GameOver:
		OnGameOver();
		break;
	}
}

void UHSAGameLoop::OnLoadingLevel()
{
	if (UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem())
	{
		LevelGen->GenerateLevel();
	}
}

void UHSAGameLoop::OnTransitioningToLevel()
{
}

void UHSAGameLoop::OnPlayingLevel()
{
}

void UHSAGameLoop::OnTransitioningOut()
{
}

void UHSAGameLoop::OnGameOver()
{
	//for now just loop
	SetState(EHSAGameState::LoadingLevel);
}

void UHSAGameLoop::OnLevelGenerated()
{
	UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem();
	if ( !ensure(LevelGen))
	{
		return ;
	}

	// Update the game instance with the new level map
	if ( UHSAGameInstance* gameInstance = Cast<UHSAGameInstance>(GetGameInstance()))
	{
		gameInstance->PopulateLevel(LevelGen->GetCurrentLevelMap());
	}
}

UHSALevelGeneration* UHSAGameLoop::GetLevelGenerationSubsystem() const
{
	return GetGameInstance()->GetSubsystem<UHSALevelGeneration>();
}