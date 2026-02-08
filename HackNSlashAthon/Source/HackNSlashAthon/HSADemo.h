#pragma once
#include "HSALevelGeneration.h"
#include "HSADemo.generated.h"

class UHSAGameLoop;

UENUM()
enum class EHSADemoState : uint8
{
	Generating,
	ShowingLevel,
	Transitioning,
	Complete
};

UCLASS()
class UHSADemo : public UObject
{
	GENERATED_BODY()

public:
	void Init(int32 LevelsCount, float LevelTime, float TransitionTime);
	void Update(float DeltaTime);

	bool IsComplete() const { return DemoState == EHSADemoState::Complete; }
	const TArray<TArray<FHSAMapTileContent>>& GetStoredLevels() const { return StoredLevels; }

protected:
	int32 LevelsCount = 0;
	float LevelTime = 0.f;
	float TransitionTime = 0.f;

	EHSADemoState DemoState = EHSADemoState::Generating;

	// Generation phase
	int32 GenerationIndex = 0;
	bool bIsGenerating = false;

	// Playback phase
	int32 PlaybackIndex = 0;
	float Timer = 0.f;

	TArray<TArray<FHSAMapTileContent>> StoredLevels;

	void UpdateGenerating();
	void UpdateShowingLevel(float DeltaTime);
	void UpdateTransitioning(float DeltaTime);

	void StartShowingNextLevel();

	UFUNCTION()
	void OnLevelGenerated();

	UHSALevelGeneration* GetLevelGenerationSubsystem() const;
	UHSAGameLoop* GetGameLoop() const;
};
