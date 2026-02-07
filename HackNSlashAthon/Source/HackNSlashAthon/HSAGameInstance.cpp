#include "HSAGameInstance.h"

#include "CoreMinimal.h"
#include "AIController.h"

#include "HackNSlashAthonGameMode.h"
#include "HSAAICharacter.h"

#include "HSAGameLoop.h"

void UHSAGameInstance::RegisterTiles( TArray<AStaticMeshActor*> InTileNames, int InRows, int InColumns)
{
	Tiles = InTileNames;
	Rows = InRows;
	Columns = InColumns;
}

void UHSAGameInstance::PopulateLevel(const TArray<int32>& LevelMap)
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
		UStaticMeshComponent* staticMeshComp = Tiles[i]->GetStaticMeshComponent();
		if ( !staticMeshComp )
		{
			continue;
		}

		staticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		staticMeshComp->SetVisibility(true);

		const EHSAEntityType EntityType = static_cast<EHSAEntityType>(LevelMap[i]);
		
		auto SpawnConfigItem = CurrentGameMode->GetSpawnConfiguration(EntityType);
		if (SpawnConfigItem == nullptr) {
			continue;
		}

		FVector SpawnLocation = Tiles[i]->GetActorLocation();
		SpawnLocation.Z += SpawnConfigItem->ActorOffsetZ + 100;

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (UHSAGameLoop::IsEnemy(EntityType))
		{
			AHSAAICharacter* AICharacter_DTO = SpawnConfigItem->SpawneableActor->GetDefaultObject<AHSAAICharacter>();


			AAIController* SpawnedController = CurrentWorld->SpawnActor<AAIController>(AICharacter_DTO->AIControllerClass, SpawnLocation, FRotator::ZeroRotator, Params);
			if (SpawnedController) {
				//SpawnLocation = Tiles[i]->GetActorLocation();
				//SpawnLocation.Z += Tiles[i]->GetSimpleCollisionHalfHeight() * 2;
				AHSAAICharacter* SpawnedActor = CurrentWorld->SpawnActor<AHSAAICharacter>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params);
				if (SpawnedActor == nullptr) {
					GetWorld()->DestroyActor(SpawnedController);
					continue;				
				}
				FVector CurrActorLocation = SpawnedActor->GetActorLocation();
				SpawnedController->Possess(SpawnedActor);
			}
			
		}

		if (EntityType == EHSAEntityType::Hole)
		{
			ECollisionEnabled::Type Collision = ECollisionEnabled::NoCollision;
			staticMeshComp->SetVisibility(false);

			SpawnedActors.Add(CurrentWorld->SpawnActor<AActor>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params));
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
