#include "HSAGameInstance.h"

#include "CoreMinimal.h"
#include "AIController.h"
#include "HackNSlashAthonCharacter.h"

#include "HackNSlashAthonGameMode.h"
#include "HSAAICharacter.h"

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
	UWorld* CurrentWorld = GetWorld();
	if (CurrentWorld == nullptr) 
	{
		return;
	}

	AHackNSlashAthonGameMode* CurrentGameMode = CurrentWorld->GetAuthGameMode<AHackNSlashAthonGameMode>();
	if (CurrentGameMode == nullptr) 
	{
		return;
	}


	for (int i = 0; i < LevelMap.Num(); i++)
	{
		auto Tile = Tiles[i];
		UStaticMeshComponent* staticMeshComp = Tile->GetStaticMeshComponent();
		if ( !staticMeshComp )
		{
			continue;
		}
		ECollisionEnabled::Type CollisionType = ECollisionEnabled::QueryAndPhysics;
		bool GroundVisibility = true;

		const EHSAEntityType EntityType = static_cast<EHSAEntityType>(LevelMap[i].EntityId);

		FVector SpawnLocation = Tile->GetActorLocation();
		// Tile start location is bottom left of the box. Z should be box height
		SpawnLocation.Z += Tile->GetSimpleCollisionHalfHeight() * 2;
		
		//reset player position
		if ( EntityType == EHSAEntityType::PlayerStart )
		{
			auto player = GetFirstLocalPlayerController();
			if ( AHackNSlashAthonCharacter* pawn = Cast<AHackNSlashAthonCharacter>(player->GetPawn()))
			{
				SpawnLocation.Z += 1500;
				pawn->SetActorLocation(SpawnLocation );
			};
			continue;
		}
		
		auto SpawnConfigItem = CurrentGameMode->GetSpawnConfiguration(EntityType);
		if (SpawnConfigItem == nullptr) {
			continue;
		}
		
		SpawnLocation.Z += SpawnConfigItem->ActorOffsetZ;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (UHSAGameLoop::IsEnemy(EntityType))
		{
			AHSAAICharacter* AICharacter_DTO = SpawnConfigItem->SpawneableActor->GetDefaultObject<AHSAAICharacter>();


			AAIController* SpawnedController = CurrentWorld->SpawnActor<AAIController>(AICharacter_DTO->AIControllerClass, SpawnLocation, FRotator::ZeroRotator, Params);
			if (SpawnedController) {
				AHSAAICharacter* SpawnedActor = CurrentWorld->SpawnActor<AHSAAICharacter>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params);
				if (SpawnedActor == nullptr) {
					GetWorld()->DestroyActor(SpawnedController);
					continue;				
				}
				FVector CurrActorLocation = SpawnedActor->GetActorLocation();
				SpawnedController->Possess(SpawnedActor);

				SpawnedActors.Add(SpawnedActor);
			}
			
		}
		else if (EntityType == EHSAEntityType::Hole)
		{
			CollisionType = ECollisionEnabled::NoCollision;
			GroundVisibility = false;

			SpawnedActors.Add(CurrentWorld->SpawnActor<AActor>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params));
		}
		else
		{
			//spawns which don't need any other configuration other than spawning
			SpawnedActors.Add(CurrentWorld->SpawnActor<AActor>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params));
		}

		staticMeshComp->SetVisibility(GroundVisibility);
		staticMeshComp->SetCollisionEnabled(CollisionType);
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
