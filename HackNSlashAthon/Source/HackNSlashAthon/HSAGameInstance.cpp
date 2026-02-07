#include "HSAGameInstance.h"

#include "HSAGameLoop.h"

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
		UStaticMeshComponent* staticMeshComp = Tiles[i]->GetStaticMeshComponent();
		if ( !staticMeshComp )
		{
			continue;
		}

		const EHSAEntityType EntityType = static_cast<EHSAEntityType>(LevelMap[i]);
		const bool IsHole = EntityType == EHSAEntityType::Hole;
		ECollisionEnabled::Type Collision = !IsHole ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
		staticMeshComp->SetVisibility(!IsHole);
		staticMeshComp->SetCollisionEnabled(Collision);

		//todo: Salva - Hook up spawning all actor types and initialize them
		
		if (UHSAGameLoop::IsEnemy(EntityType))
		{
			//FActorSpawnParameters SpawnParams;
			//GetWorld()->SpawnActor();
		}
	}
}

void UHSAGameInstance::CleanLevel()
{
	for (auto Actor : SpawnedActors)
	{
		Actor->Destroy();
	}
	SpawnedActors.Empty();
}
