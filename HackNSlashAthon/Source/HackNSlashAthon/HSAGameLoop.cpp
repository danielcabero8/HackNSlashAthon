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
	case EHSAGameState::LoadingComplete:
		OnLoadingComplete();
		break;
	case EHSAGameState::TransitioningIn:
		OnTransitioningIn();
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
	case EHSAGameState::LevelCompleted:
		OnLevelCompleted();
		break;
	}

	OnGameStateChanged.Broadcast(CurrentState);
}

void UHSAGameLoop::HitPlayer()
{
	GameLevelData.CurrentLives--;
	DataUpdated();
}

void UHSAGameLoop::EnemyKilled(AActor* Enemy)
{
	GameLevelData.EnemiesAlive--;
	GameLevelData.EnemiesKilled++;
	DataUpdated();
}

void UHSAGameLoop::OnLoadingLevel()
{
	if (UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem())
	{
		LevelGen->GenerateLevel();
	}
}

void UHSAGameLoop::OnLoadingComplete()
{
}

void UHSAGameLoop::OnTransitioningIn()
{
}

void UHSAGameLoop::OnPlayingLevel()
{
}

void UHSAGameLoop::OnTransitioningOut()
{
	//IMPORTANT: Transition to LoadingLevel happens in Map's LevelBP
}

void UHSAGameLoop::OnGameOver()
{
	//for now just loop
	SetState(EHSAGameState::TransitioningOut);
}

void UHSAGameLoop::OnLevelCompleted()
{
	//for now just loop
	SetState(EHSAGameState::TransitioningOut);
}

void UHSAGameLoop::DataUpdated()
{
	if ( GameLevelData.CurrentLives <= 0 )
	{
		SetState(EHSAGameState::GameOver);
	}
	else if ( GameLevelData.EnemiesAlive <= 0 )
	{
		SetState(EHSAGameState::LevelCompleted);
	}
}

void UHSAGameLoop::OnLevelGenerated()
{
	UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem();
	if ( !ensure(LevelGen))
	{
		return ;
	}

	// Reset the level data
	GameLevelData = FGameLevelData();

	// Update the game instance with the new level map
	if ( UHSAGameInstance* gameInstance = Cast<UHSAGameInstance>(GetGameInstance()))
	{
		//spawn entities and floor
		auto LevelMap = LevelGen->GetCurrentLevelMap();
		gameInstance->PopulateLevel(LevelMap);

		auto LevelMapNEW = LevelGen->GetCurrentLevelMapNEW();
		
		
		//register the amount of enemies in this level
		for (int i = 0; i < LevelMapNEW.Num(); i++)
		{
			if (IsEnemy(static_cast<EEntityType>(LevelMapNEW[i].EntityId)))
			{
				GameLevelData.EnemiesAlive++;
			}
		}
	}

	SetState(EHSAGameState::LoadingComplete);
}

UHSALevelGeneration* UHSAGameLoop::GetLevelGenerationSubsystem() const
{
	return GetGameInstance()->GetSubsystem<UHSALevelGeneration>();
}


bool UHSAGameLoop::IsEnemy(const EEntityType EntityType)
{
	return EntityType > EEntityType::ENEMIES && EntityType < EEntityType::TRAPS;
}

bool UHSAGameLoop::IsEnvironment(const EEntityType EntityType)
{
	return EntityType > EEntityType::ENVIRONMENT;
}

bool UHSAGameLoop::IsTrap(const EEntityType EntityType)
{
	return EntityType > EEntityType::TRAPS && EntityType < EEntityType::ENVIRONMENT;
}