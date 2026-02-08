#pragma once
#include "HSALevelGeneration.h"
#include "HSAGameLoop.generated.h"

UENUM(BlueprintType)
enum class EHSAEntityType : uint8
{
	//general
	GENERAL				= 0 UMETA(Hidden),
	Hole				= 1,
	Floor				= 2,
	PlayerStart			= 3,

	//enemies
	ENEMIES				= 10 UMETA(Hidden),
	EnemyType1			= 11,

	//traps
	TRAPS				= 20 UMETA(Hidden),
	Spikes				= 21,

	//Environment
	ENVIRONMENT			= 30 UMETA(Hidden),
	Column				= 31,
};


UENUM(BlueprintType)
enum class EHSAGameState : uint8
{
	LoadingLevel,
	LoadingComplete,
	PlayingLevel,

	TransitioningIn,
	TransitioningOut,

	LevelCompleted,
	GameOver
};

USTRUCT(BlueprintType)
struct FHSAGameLevelData
{
	GENERATED_BODY()

	UPROPERTY(blueprintReadOnly)
	int CurrentLives = 3;

	UPROPERTY(blueprintReadOnly)
	int EnemiesKilled = 0;

	UPROPERTY(blueprintReadOnly)
	int EnemiesAlive = 0;

	UPROPERTY(blueprintReadOnly)
	int HitsTaken = 0;

	UPROPERTY(blueprintReadOnly)
	float TimeElapsed = 0.f;
};

class UHSADemo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EHSAGameState, NewState);

UCLASS()
class UHSAGameLoop : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly)
	FHSAGameLevelData GameLevelData;

	UPROPERTY(BlueprintReadOnly)
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

	void PrepareLevel(const TArray<FHSAMapTileContent>& LevelMap);
	
	UPROPERTY(BlueprintAssignable)
	FOnGameStateChanged OnGameStateChanged;

	UFUNCTION(BlueprintCallable)
	void SetState( EHSAGameState state);

	UFUNCTION(BlueprintCallable)
	void StartDemo(int32 LevelsCount, float LevelTime, float TransitionTime);

	//Api
	UFUNCTION(BlueprintCallable)
	void HitPlayer();

	UFUNCTION(BlueprintCallable)
	void EnemyKilled(AActor* Enemy);

	static bool IsEnemy(const EHSAEntityType EntityType);
	static bool IsEnvironment(const EHSAEntityType EntityType);
	static bool IsTrap(const EHSAEntityType EntityType);
	
protected:
	UPROPERTY(BlueprintReadOnly)
	EHSAGameState CurrentState;

	void OnLoadingLevel();
	void OnLoadingComplete();
	void OnTransitioningIn();
	void OnPlayingLevel();
	void OnTransitioningOut();
	void OnGameOver();
	void OnLevelCompleted();
	
	void DataUpdated();

	UFUNCTION()
	void OnLevelGenerated();
	
	UPROPERTY()
	TObjectPtr<UHSADemo> Demo;

	//helpers
	UHSALevelGeneration* GetLevelGenerationSubsystem() const;
};
