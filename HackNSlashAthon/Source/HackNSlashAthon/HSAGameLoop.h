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
	TransitioningToLevel,
	PlayingLevel,
	TransitioningOut,
	
	LevelCompleted,
	GameOver
};

USTRUCT(BlueprintType)
struct FHSAGameLevelData
{
	GENERATED_BODY()

	int CurrentLives = 3;
	int EnemiesKilled = 0;
	int EnemiesAlive = 0;
	int HitsTaken = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EHSAGameState, NewState);

UCLASS()
class UHSAGameLoop : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:

	FHSAGameLevelData GameLevelData;
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

	UPROPERTY(BlueprintAssignable)
	FOnGameStateChanged OnGameStateChanged;

	UFUNCTION(BlueprintCallable)
	void SetState( EHSAGameState state);



	//Api
	UFUNCTION(BlueprintCallable)
	void HitPlayer();

	UFUNCTION(BlueprintCallable)
	void EnemyKilled(AActor* Enemy);

	static bool IsEnemy(const EHSAEntityType EntityType);
	static bool IsEnvironment(const EHSAEntityType EntityType);
	static bool IsTrap(const EHSAEntityType EntityType);
	
private:
	EHSAGameState CurrentState;

	void OnLoadingLevel();
	void OnTransitioningToLevel();
	void OnPlayingLevel();
	void OnTransitioningOut();
	void OnGameOver();

	void FadeToBlack();
	void FadeOutToGame();

	void DataUpdated();

	UFUNCTION()
	void OnLevelGenerated();
	
	//helpers
	UHSALevelGeneration* GetLevelGenerationSubsystem() const;
};
