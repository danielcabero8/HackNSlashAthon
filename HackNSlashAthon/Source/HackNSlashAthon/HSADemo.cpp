#include "HSADemo.h"

#include "HSAGameInstance.h"
#include "HSAGameLoop.h"

void UHSADemo::Init(int32 InLevelsCount, float InLevelTime, float InTransitionTime)
{
	LevelsCount = InLevelsCount;
	LevelTime = InLevelTime;
	TransitionTime = InTransitionTime;

	StoredLevels.Empty();
	StoredLevels.Reserve(LevelsCount);
	GenerationIndex = 0;
	bIsGenerating = false;
	PlaybackIndex = 0;
	Timer = 0.f;
	DemoState = EHSADemoState::Generating;

	if (UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem())
	{
		LevelGen->OnLevelGenerated.AddDynamic(this, &UHSADemo::OnLevelGenerated);
	}
}

void UHSADemo::Update(float DeltaTime)
{
	switch (DemoState)
	{
	case EHSADemoState::Generating:
		UpdateGenerating();
		break;
	case EHSADemoState::ShowingLevel:
		UpdateShowingLevel(DeltaTime);
		break;
	case EHSADemoState::Transitioning:
		UpdateTransitioning(DeltaTime);
		break;
	case EHSADemoState::Complete:
		break;
	}
}

void UHSADemo::UpdateGenerating()
{
	if (bIsGenerating)
	{
		return;
	}

	if (GenerationIndex >= LevelsCount)
	{
		// All levels generated, start playback
		StartShowingNextLevel();
		return;
	}

	UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem();
	if (!LevelGen)
	{
		return;
	}

	// Build level generation data with incrementing level and fake stats
	UHSAGameInstance* GameInstance = GetTypedOuter<UHSAGameInstance>();

	FHSALevelGenerationData Data;
	Data.CompletedLevel = GenerationIndex + 1;
	Data.TimeTaken = FMath::RandRange(20.f, 60.f);
	Data.HitsTaken = FMath::RandRange(0, 2);
	Data.EnemiesKilled = FMath::RandRange(1, 5);

	if (GameInstance)
	{
		Data.Rows = GameInstance->Rows;
		Data.Columns = GameInstance->Columns;
	}

	bIsGenerating = true;
	LevelGen->GenerateLevel(Data);
}

void UHSADemo::UpdateShowingLevel(float DeltaTime)
{
	Timer -= DeltaTime;
	if (Timer <= 0.f)
	{
		if (UHSAGameLoop* GameLoop = GetGameLoop())
		{
			GameLoop->SetState(EHSAGameState::TransitioningOut);
		}
		Timer = TransitionTime;
		DemoState = EHSADemoState::Transitioning;
	}
}

void UHSADemo::UpdateTransitioning(float DeltaTime)
{
	Timer -= DeltaTime;
	if (Timer <= 0.f)
	{
		PlaybackIndex++;
		if (PlaybackIndex < StoredLevels.Num())
		{
			StartShowingNextLevel();
		}
		else
		{
			DemoState = EHSADemoState::Complete;
		}
	}
}

void UHSADemo::StartShowingNextLevel()
{
	if (UHSAGameLoop* GameLoop = GetGameLoop())
	{
		GameLoop->PrepareLevel(StoredLevels[PlaybackIndex]);
	}
	Timer = LevelTime;
	DemoState = EHSADemoState::ShowingLevel;
}

void UHSADemo::OnLevelGenerated()
{
	UHSALevelGeneration* LevelGen = GetLevelGenerationSubsystem();
	if (!LevelGen)
	{
		return;
	}

	StoredLevels.Add(LevelGen->GetCurrentLevelMap());
	GenerationIndex++;
	bIsGenerating = false;

	if (GenerationIndex >= LevelsCount)
	{
		LevelGen->OnLevelGenerated.RemoveDynamic(this, &UHSADemo::OnLevelGenerated);
	}
}

UHSALevelGeneration* UHSADemo::GetLevelGenerationSubsystem() const
{
	UGameInstance* GI = GetTypedOuter<UGameInstance>();
	return GI ? GI->GetSubsystem<UHSALevelGeneration>() : nullptr;
}

UHSAGameLoop* UHSADemo::GetGameLoop() const
{
	return Cast<UHSAGameLoop>(GetOuter());
}
