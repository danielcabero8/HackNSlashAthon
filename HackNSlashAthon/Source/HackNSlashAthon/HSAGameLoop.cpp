#include "HSAGameLoop.h"

#include "HackNSlashAthonCharacter.h"
#include "HackNSlashAthonPlayerController.h"
#include "HSADemo.h"
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

void UHSAGameLoop::PrepareLevel(const TArray<FHSAMapTileContent>& LevelMap)
{
	// Reset the level data
	GameLevelData = FHSAGameLevelData();
	CurrentDungeonLevel++;
	
	// Update the game instance with the new level map
	if ( UHSAGameInstance* gameInstance = Cast<UHSAGameInstance>(GetGameInstance()))
	{
		//spawn entities and floor
		gameInstance->PopulateLevel(LevelMap);

		//register the amount of enemies in this level
		for (int i = 0; i < LevelMap.Num(); i++)
		{
			if (IsEnemy(static_cast<EHSAEntityType>(LevelMap[i].EntityId)))
			{
				GameLevelData.EnemiesAlive++;
			}
		}
	}

	SetState(EHSAGameState::LoadingComplete);
}

void UHSAGameLoop::Tick(float DeltaTime)
{
	if (Demo)
	{
		Demo->Update(DeltaTime);
	}
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

void UHSAGameLoop::StartDemo(int32 LevelsCount, float LevelTime, float TransitionTime)
{
	Demo = NewObject<UHSADemo>(this);
	Demo->Init(LevelsCount, LevelTime, TransitionTime);
}

void UHSAGameLoop::HitPlayer()
{
	GameLevelData.CurrentLives--;
	DataUpdated();
}

void UHSAGameLoop::KillPlayer()
{
	GameLevelData.CurrentLives = 0;
	DataUpdated();
}

void UHSAGameLoop::EnemyKilled(AActor* Enemy)
{
	GameLevelData.EnemiesAlive--;
	GameLevelData.EnemiesKilled++;
	
	Enemy->Destroy();

	DataUpdated();
}

void UHSAGameLoop::OnLoadingLevel()
{
	if (UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem())
	{
		UHSAGameInstance* GameInstance = Cast<UHSAGameInstance>(GetGameInstance());
		if (!ensure(GameInstance))
		{
			return;
		}

		FHSALevelGenerationData LevelGenData;
		LevelGenData.Rows = GameInstance->Rows;
		LevelGenData.Columns = GameInstance->Columns;
		LevelGenData.CompletedLevel = CurrentDungeonLevel;
		LevelGenData.TimeTaken = GameLevelData.TimeElapsed;
		LevelGenData.HitsTaken = GameLevelData.HitsTaken;
		LevelGenData.EnemiesKilled = GameLevelData.EnemiesKilled;

		LevelGen->GenerateLevel(LevelGenData);
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
	CurrentDungeonLevel = 0;
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
	if (Demo != nullptr)
	{
		return;
	}

	UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem();
	if ( ensure(LevelGen) )
	{
		PrepareLevel(LevelGen->GetCurrentLevelMap());
	}
}

UHSALevelGeneration* UHSAGameLoop::GetLevelGenerationSubsystem() const
{
	return GetGameInstance()->GetSubsystem<UHSALevelGeneration>();
}


bool UHSAGameLoop::IsEnemy(const EHSAEntityType EntityType)
{
	return EntityType > EHSAEntityType::ENEMIES && EntityType < EHSAEntityType::TRAPS;
}

bool UHSAGameLoop::IsEnvironment(const EHSAEntityType EntityType)
{
	return EntityType > EHSAEntityType::ENVIRONMENT;
}

bool UHSAGameLoop::IsTrap(const EHSAEntityType EntityType)
{
	return EntityType > EHSAEntityType::TRAPS && EntityType < EHSAEntityType::ENVIRONMENT;
}