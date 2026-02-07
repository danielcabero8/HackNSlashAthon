#pragma once
#include "Engine/StaticMeshActor.h"
#include "HSAGameInstance.generated.h"

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
	TArray<AStaticMeshActor*> Tiles;
	
	UFUNCTION(blueprintCallable)
	void RegisterTiles( TArray<AStaticMeshActor*> InTileNames, int InRows, int InColumns);

	UFUNCTION(BlueprintCallable)
	void PopulateLevel(const TArray<int32>& LevelMap);

	//remove all actor generated from the previous level
	UFUNCTION(BlueprintCallable)
	void CleanLevel();
};
