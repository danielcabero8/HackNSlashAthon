#include "HSAGameInstance.h"

#include "CoreMinimal.h"
#include "HackNSlashAthonGameMode.h"
#include "HSAGameLoop.h"

void UHSAGameInstance::RegisterTiles( TArray<AStaticMeshActor*> InTileNames, int InRows, int InColumns)
{
	Tiles = InTileNames;
	Rows = InRows;
	Columns = InColumns;

	auto GetTrailingNumber = [](const FString& Label) -> int32
	{
		const int32 LastNonDigit = Label.FindLastCharByPredicate([](TCHAR C) { return !FChar::IsDigit(C); });
		const FString NumberPart = Label.RightChop(LastNonDigit + 1);
		return FCString::Atoi(*NumberPart);
	};

	Tiles.Sort([&GetTrailingNumber](const AStaticMeshActor& A, const AStaticMeshActor& B) -> bool
	{
		const int32 NumA = GetTrailingNumber(A.GetActorLabel());
		const int32 NumB = GetTrailingNumber(B.GetActorLabel());
		return NumA < NumB;
	});
}

void UHSAGameInstance::PopulateLevel(const TArray<FHSAMapTileContent>& LevelMap)
{
	for (int i = 0; i < LevelMap.Num(); i++)
	{
		UStaticMeshComponent* staticMeshComp = Tiles[i]->GetStaticMeshComponent();
		if ( !staticMeshComp )
		{
			continue;
		}

		const EHSAEntityType EntityType = static_cast<EHSAEntityType>(LevelMap[i].EntityId);
		const bool IsHole = EntityType == EHSAEntityType::Hole;
		ECollisionEnabled::Type Collision = !IsHole ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
		staticMeshComp->SetVisibility(!IsHole);
		staticMeshComp->SetCollisionEnabled(Collision);

		//todo: Salva - Hook up spawning all actor types and initialize them
		
		if (UHSAGameLoop::IsEnemy(EntityType))
		{
			AHackNSlashAthonGameMode* CurrentGameMode = GetWorld()->GetAuthGameMode<AHackNSlashAthonGameMode>();
			if (CurrentGameMode != nullptr) {
				return;
			}


			TSubclassOf<AActor> ActorToSpawn = CurrentGameMode->GetActorToSpawn(EntityType);
			
			FActorSpawnParameters Params;			
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Tiles[i]->GetActorLocation(), FRotator::ZeroRotator, Params);

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
