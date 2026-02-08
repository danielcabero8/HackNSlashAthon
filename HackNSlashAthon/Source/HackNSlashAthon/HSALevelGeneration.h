#pragma once
#include "Interfaces/IHttpRequest.h"
#include "HSALevelGeneration.generated.h"

USTRUCT(BlueprintType)
struct FHSAMapTileContent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 EntityId = 0;

	UPROPERTY(BlueprintReadWrite)
	float MovementSpeed = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AttackCadence = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float BulletSpeed = 0.f;
};


USTRUCT(BlueprintType)
struct FHSALevelGenerationData
{
	GENERATED_BODY()

	int Rows, Columns;
	int HitsTaken;
	float TimeTaken;
	int CompletedLevel;
	int EnemiesKilled;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelGenerated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClaudeResponseReceived, const FString&, Response);

UCLASS()
class UHSALevelGeneration : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnLevelGenerated OnLevelGenerated;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const TArray<FHSAMapTileContent>& GetCurrentLevelMap() const { return MapTileContents; }
	void GenerateLevel(const FHSALevelGenerationData& Data);

	/**
	 * Send a prompt to the Claude API asynchronously.
	 * @param Prompt - The user message to send.
	 * @param MaxTokens - Maximum tokens in the response (default 1024).
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelGeneration|Claude")
	void SendClaudeRequest(const FString& Prompt, int32 MaxTokens = 4096);

	/** API key for the Anthropic Claude API. Set this before calling SendClaudeRequest. */
	UPROPERTY(BlueprintReadWrite, Category = "LevelGeneration|Claude")
	FString ClaudeApiKey;

	/** Claude model to use (e.g. "claude-sonnet-4-5-20250929"). */
	UPROPERTY(BlueprintReadWrite, Category = "LevelGeneration|Claude")
	FString ClaudeModel = TEXT("claude-haiku-4-5-20251001");

	/** System prompt sent with every Claude request. */
	UPROPERTY(BlueprintReadWrite, Category = "LevelGeneration|Claude")
	FString SystemPrompt;

protected:
	/** Stored level generation data from the last GenerateLevel call (used by async response handler). */
	FHSALevelGenerationData PendingLevelGenData;

	/** Parsed map tile contents from the last Claude response. */
	UPROPERTY(BlueprintReadOnly, Category = "LevelGeneration|Claude")
	TArray<FHSAMapTileContent> MapTileContents;
	
	/** HTTP response callback bound to each request. */
	void OnClaudeHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
