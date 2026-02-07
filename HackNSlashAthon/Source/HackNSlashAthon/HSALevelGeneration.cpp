#include "HSALevelGeneration.h"
#include "HSAGameLoop.h"
#include "HSAGameInstance.h"
#include "HSAGameLoop.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"

class UHSAGameLoop;

void UHSALevelGeneration::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Load Claude API key from file (ProjectDir/claude_api_key.txt)
	const FString KeyFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("claude_api_key.txt"));
	if (FFileHelper::LoadFileToString(ClaudeApiKey, *KeyFilePath))
	{
		ClaudeApiKey.TrimStartAndEndInline();
		UE_LOG(LogTemp, Log, TEXT("HSALevelGeneration: Loaded Claude API key from %s"), *KeyFilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Could not load Claude API key from %s. Set ClaudeApiKey manually or create the file."), *KeyFilePath);
	}

	SystemPrompt = TEXT(
		"You are a game level generator. The game is a top down hack n slash game, where the layout is tile based in rows and columns MxN. "
		"Your job is to decide what is the layout MxN that the next level will have. The information in each entry of MxN defines what entity spawns in that tile. "

		"Assumptions: "
		"> Floor is always spawned underneath every tile, UNLESS you decide the tile is \"Hole\". If you decide Floor, it will only spawn a floor tile, with no other entity above. "
		"> Your role is to generate level layout BUT it is NOT random - your purpose as the game level generator is to generate increasingly more difficult levels, as the player keeps completing levels. "
		"> Any tile not explicitly assigned a special entity defaults to Floor. "

		"User prompt format: "
		"The user prompt will include information about the recently completed level as a JSON object with the following fields: "
		"\"CompletedLevel\" (integer) - the current level that the player has completed. "
		"\"Time\" (float) - the time in seconds that the player took to complete the level. "
		"\"HitsTaken\" (integer) - how many hits the player has taken before completing the level. The player starts with 3 lives. If \"HitsTaken\" is 0, it means they completed the level with 3 lives still. If \"HitsTaken\" is 2 it means they completed the level with 1 life left, only 1 hit away from Game Over. "
		"\"Rows\" (integer) - the number of rows in the grid. "
		"\"Columns\" (integer) - the number of columns in the grid. "
		"Example user prompt: "
		"{\"CompletedLevel\":1,\"Time\":32.5,\"HitsTaken\":1,\"Rows\":5,\"Columns\":5} "

		"Entity definitions (ordered by category): "
		"Category: GENERAL - general entity types with different functionality. "
		"  Hole = 1: Empty space where enemies and the player can fall, meaning they die. For the player, dying means Game Over. "
		"  Floor = 2: An empty traversable space for both enemies and the player. "
		"  PlayerStart = 3: The tile where the player will start the level. "
		"Category: ENEMIES - enemy types controlled by AI that the player must defeat. "
		"  EnemyType1 = 11: A medium difficulty enemy. "
		"Category: TRAPS - environmental entities that harm the player, reducing life by 1 hit. "
		"  Spikes = 21: A trap that damages the player on contact. "
		"Category: ENVIRONMENT - obstacles that can act as cover for the player and give variety to the level. "
		"  Column = 31: A tall column that blocks movement and projectiles. "

		"Constraints: "
		"> Every level must contain exactly one PlayerStart tile (EntityId 3). "
		"> Tile indices are 0-based in row-major order: index 0 is top-left, index Columns-1 is top-right, index Columns is second row first tile, etc. Total tiles = Rows * Columns. "
		"> MovementSpeed, AttackCadence, and BulletSpeed are only relevant for ENEMY entities. "

		"Win and game conditions: "
		"> The player wins a level by killing all enemies in the level. "
		"> The game has no final end - the goal is to see how many levels the player can beat before getting Game Over. "

		"Balancing baseline - the first level (easiest) should consist of: "
		"> 2 enemies "
		"> 1 environment "
		"> 5 holes "
		"> All remaining tiles default to Floor. "

		"Response format (IMPORTANT - use sparse format to minimize tokens): "
		"Respond with ONLY valid JSON, no extra text or markdown. "
		"The JSON must contain a single key \"M\" whose value is an object mapping tile index (as string) to tile data. "
		"Only include non-Floor tiles. Any tile index not present defaults to Floor (EntityId 2). "
		"For non-enemy tiles: the value is just the EntityId as a number. "
		"For enemy tiles: the value is an array [EntityId, MovementSpeed, AttackCadence, BulletSpeed]. "
		"Example response for a 5x5 grid: "
		"{\"M\":{\"0\":1,\"3\":[11,300.0,1.5,800.0],\"7\":3,\"12\":1,\"18\":21,\"22\":31}}"
	);
}

void UHSALevelGeneration::GenerateLevel()
{
	UHSAGameInstance* gameInstance = Cast<UHSAGameInstance>(GetWorld()->GetGameInstance());
	if ( !ensure(gameInstance))
	{
		return;
	}

	UHSAGameLoop* gameLoop = gameInstance->GetSubsystem<UHSAGameLoop>();
	if ( !ensure(gameLoop))
	{
		return;
	}

	//debug version
	const int32 mapSize = gameInstance->Columns * gameInstance->Rows;
	CurrentLevelMap = TArray<int32>();
	CurrentLevelMap.Reserve(mapSize);
	
	for ( int i = 0; i < mapSize; i++ )
	{
		const int value = FMath::RandRange(0, 1);
		CurrentLevelMap.Add(value);
	}

	CurrentLevelMap[8] = static_cast<int32>(EHSAEntityType::EnemyType1);
	CurrentLevelMap[16] = static_cast<int32>(EHSAEntityType::EnemyType1);
	
	//real version
	const int32 CompletedLevel = gameLoop->CurrentDungeonLevel;
	const FHSAGameLevelData& CompletedLevelData = gameLoop->GameLevelData;

	// Build user prompt JSON
	TSharedPtr<FJsonObject> PromptJson = MakeShareable(new FJsonObject());
	PromptJson->SetNumberField(TEXT("CompletedLevel"), CompletedLevel);
	PromptJson->SetNumberField(TEXT("Time"), CompletedLevelData.TimeElapsed);
	PromptJson->SetNumberField(TEXT("HitsTaken"), CompletedLevelData.HitsTaken);
	PromptJson->SetNumberField(TEXT("Rows"), gameInstance->Rows);
	PromptJson->SetNumberField(TEXT("Columns"), gameInstance->Columns);

	FString PromptString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&PromptString);
	FJsonSerializer::Serialize(PromptJson.ToSharedRef(), Writer);

	SendClaudeRequest(PromptString);
}

void UHSALevelGeneration::SendClaudeRequest(const FString& Prompt, int32 MaxTokens)
{
	if (ClaudeApiKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: ClaudeApiKey is not set."));
		return;
	}

	// Build the JSON request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject());
	RequestBody->SetStringField(TEXT("model"), ClaudeModel);
	RequestBody->SetNumberField(TEXT("max_tokens"), MaxTokens);

	// System prompt (optional)
	if (!SystemPrompt.IsEmpty())
	{
		RequestBody->SetStringField(TEXT("system"), SystemPrompt);
	}

	// Messages array with user prompt
	TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject());
	UserMessage->SetStringField(TEXT("role"), TEXT("user"));
	UserMessage->SetStringField(TEXT("content"), Prompt);

	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	MessagesArray.Add(MakeShareable(new FJsonValueObject(UserMessage)));
	RequestBody->SetArrayField(TEXT("messages"), MessagesArray);

	// Serialize to string
	FString RequestBodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBodyString);
	FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);

	// Create and configure HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(TEXT("https://api.anthropic.com/v1/messages"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("x-api-key"), ClaudeApiKey);
	HttpRequest->SetHeader(TEXT("anthropic-version"), TEXT("2023-06-01"));
	HttpRequest->SetContentAsString(RequestBodyString);
	HttpRequest->SetTimeout( 120.0f );

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHSALevelGeneration::OnClaudeHttpResponseReceived);
	HttpRequest->ProcessRequest();

	UE_LOG(LogTemp, Log, TEXT("HSALevelGeneration: Claude API request sent."));
}

void UHSALevelGeneration::OnClaudeHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (!bConnectedSuccessfully || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Claude API request failed - no connection."));
		return;
	}

	const int32 ResponseCode = Response->GetResponseCode();
	const FString ResponseBody = Response->GetContentAsString();

	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Claude API returned HTTP %d: %s"), ResponseCode, *ResponseBody);
		return;
	}

	// Parse JSON response
	TSharedPtr<FJsonObject> JsonResponse;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
	if (!FJsonSerializer::Deserialize(Reader, JsonResponse) || !JsonResponse.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Failed to parse Claude API response JSON."));
		return;
	}

	// Extract text from content array: response.content[0].text
	const TArray<TSharedPtr<FJsonValue>>* ContentArray;
	if (!JsonResponse->TryGetArrayField(TEXT("content"), ContentArray) || ContentArray->Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Claude API response has no content."));
		return;
	}

	const TSharedPtr<FJsonObject>* FirstContentBlock;
	if (!(*ContentArray)[0]->TryGetObject(FirstContentBlock))
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Could not read content block."));
		return;
	}

	FString TextContent;
	if (!(*FirstContentBlock)->TryGetStringField(TEXT("text"), TextContent))
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Content block has no text field."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("HSALevelGeneration: Claude response received (%d chars)."), TextContent.Len());

	// Strip markdown code fences if present (e.g. ```json ... ```)
	TextContent.TrimStartAndEndInline();
	if (TextContent.StartsWith(TEXT("```")))
	{
		int32 FirstNewline;
		if (TextContent.FindChar(TEXT('\n'), FirstNewline))
		{
			TextContent.RightChopInline(FirstNewline + 1);
		}
		if (TextContent.EndsWith(TEXT("```")))
		{
			TextContent.LeftChopInline(3);
		}
		TextContent.TrimStartAndEndInline();
	}

	// Parse TextContent as JSON and extract sparse map "M"
	TSharedPtr<FJsonObject> MapJson;
	TSharedRef<TJsonReader<>> MapReader = TJsonReaderFactory<>::Create(TextContent);
	if (!FJsonSerializer::Deserialize(MapReader, MapJson) || !MapJson.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: Claude response text is not valid JSON."));
		return;
	}

	const TSharedPtr<FJsonObject>* SparseMap;
	if (!MapJson->TryGetObjectField(TEXT("M"), SparseMap))
	{
		UE_LOG(LogTemp, Error, TEXT("HSALevelGeneration: JSON has no \"M\" object."));
		return;
	}

	// Get grid dimensions from game instance
	UHSAGameInstance* GameInstance = Cast<UHSAGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		return;
	}
	const int32 TotalTiles = GameInstance->Rows * GameInstance->Columns;

	// Initialize all tiles as Floor (EntityId 2)
	MapTileContents.Empty();
	MapTileContents.SetNum(TotalTiles);
	for (FHSAMapTileContent& Tile : MapTileContents)
	{
		Tile.EntityId = 2;
		Tile.MovementSpeed = 0.f;
		Tile.AttackCadence = 0.f;
		Tile.BulletSpeed = 0.f;
	}

	// Overlay sparse entries onto the grid
	for (const auto& Pair : (*SparseMap)->Values)
	{
		const int32 Index = FCString::Atoi(*Pair.Key);
		if (Index < 0 || Index >= TotalTiles)
		{
			UE_LOG(LogTemp, Warning, TEXT("HSALevelGeneration: Tile index %d out of range (0-%d), skipping."), Index, TotalTiles - 1);
			continue;
		}

		FHSAMapTileContent& Tile = MapTileContents[Index];

		// Enemy tiles come as array: [EntityId, MovementSpeed, AttackCadence, BulletSpeed]
		const TArray<TSharedPtr<FJsonValue>>* TileArray;
		if (Pair.Value->TryGetArray(TileArray) && TileArray->Num() == 4)
		{
			Tile.EntityId = static_cast<int32>((*TileArray)[0]->AsNumber());
			Tile.MovementSpeed = static_cast<float>((*TileArray)[1]->AsNumber());
			Tile.AttackCadence = static_cast<float>((*TileArray)[2]->AsNumber());
			Tile.BulletSpeed = static_cast<float>((*TileArray)[3]->AsNumber());
		}
		else
		{
			// Non-enemy tile: value is just the EntityId
			Tile.EntityId = static_cast<int32>(Pair.Value->AsNumber());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("HSALevelGeneration: Parsed %d tiles (%d non-floor) from sparse map."), TotalTiles, (*SparseMap)->Values.Num());

	// Broadcast the raw response text to listeners
	OnLevelGenerated.Broadcast();
}
