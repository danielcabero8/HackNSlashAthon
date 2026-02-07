#pragma once
#include "HSALevelGeneration.h"
#include "HSAGameLoop.generated.h"

UENUM(BlueprintType)
enum class EHSAGameState : uint8
{
	LoadingLevel,
	TransitioningToLevel,
	PlayingLevel,
	TransitioningOut,
	GameOver
};

UCLASS()
class UHSAGameLoop : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:

	int CurrentDungeonLevel = 1;

	// --- FTickableGameObject Overrides ---
	// This is your actual Tick function
	virtual void Tick(float DeltaTime) override;

	// Determines if this object is allowed to tick right now
	virtual bool IsTickable() const override;

	// Returns the Stat ID for profiling (standard boilerplate)
	virtual TStatId GetStatId() const override;

	// Ensures the object doesn't tick inside the Editor preview if you don't want it to
	virtual bool IsTickableInEditor() const override { return false; }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void SetState( EHSAGameState state);

private:



	EHSAGameState CurrentState;

	void OnLoadingLevel();
	void OnTransitioningToLevel();
	void OnPlayingLevel();
	void OnTransitioningOut();
	void OnGameOver();

	void FadeToBlack();
	void FadeOutToGame();


	UFUNCTION()
	void OnLevelGenerated();
	
	//helpers
	UHSALevelGeneration* GetLevelGenerationSubsystem() const;
};
