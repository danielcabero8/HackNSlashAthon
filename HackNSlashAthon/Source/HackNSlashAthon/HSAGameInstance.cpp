#include "HSAGameInstance.h"

void UHSAGameInstance::RegisterTiles( TArray<AStaticMeshActor*> InTileNames, int InRows, int InColumns)
{
	Tiles = InTileNames;
	Rows = InRows;
	Columns = InColumns;
}

void UHSAGameInstance::PopulateLevel(const TArray<int32>& LevelMap)
{
	for (int i = 0; i < LevelMap.Num(); i++)
	{
		ECollisionEnabled::Type Collision = LevelMap[i] == 1 ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

		if ( UStaticMeshComponent* staticMeshComp = Tiles[i]->GetStaticMeshComponent())
		{
			staticMeshComp->SetVisibility(LevelMap[i] == 1);
			staticMeshComp->SetCollisionEnabled(Collision);
		}
	}
}

void UHSAGameInstance::CleanLevel()
{
	//
}
