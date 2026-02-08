#pragma once
#include "Engine/StaticMeshActor.h"
#include "AIController.h"
#include "HSAGameInstance.generated.h"

class AHSAAICharacter;

USTRUCT(BlueprintType) // Required for BP to recognize the struct
struct FMapGridRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AStaticMeshActor*> Tiles;
};

UCLASS()
class UHSAGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	int Rows, Columns;
	
	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	UPROPERTY()
	TArray<AStaticMeshActor*> Tiles;
	
	// Delay (seconds) applied after level generation before AI controllers are spawned and possess enemies.
	// You can tweak this in editor (class defaults of your GameInstance Blueprint) to add extra wait time.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AIStartDelay = 0.5f;

	UFUNCTION(blueprintCallable)
	void RegisterTiles( TArray<AStaticMeshActor*> InTileNames, int InRows, int InColumns);

	UFUNCTION(BlueprintCallable)
	void PopulateLevel(const TArray<FHSAMapTileContent>& LevelMap);

	//remove all actor generated from the previous level
	UFUNCTION(BlueprintCallable)
	void CleanLevel();

	// Timer callback to spawn a controller and possess the pawn after AIStartDelay
	UFUNCTION()
	void SpawnAndPossessController(AHSAAICharacter* Pawn, TSubclassOf<AAIController> ControllerClass);

};
