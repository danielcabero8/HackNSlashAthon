#include "HSAGameInstance.h"

#include "CoreMinimal.h"
#include "HackNSlashAthonCharacter.h"

#include "HackNSlashAthonGameMode.h"
#include "HSAAICharacter.h"

#include "HSAGameLoop.h"
#include "Components/BoxComponent.h"

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
		UBoxComponent* boxCollision = Tile->GetComponentByClass<UBoxComponent>();
		if ( !staticMeshComp)
		{
			continue;
		}
		ECollisionEnabled::Type CollisionType = ECollisionEnabled::QueryAndPhysics;
		bool GroundVisibility = true;

		const EHSAEntityType EntityType = static_cast<EHSAEntityType>(LevelMap[i].EntityId);

		FVector SpawnLocation = Tile->GetActorLocation();
		// Tile start location is bottom left of the box. Z should be box height
		SpawnLocation.Z += Tile->GetSimpleCollisionHalfHeight();
		SpawnLocation.X -= 50.0;
		SpawnLocation.Y -= 50.0;
		
		//reset player position
		if ( EntityType == EHSAEntityType::PlayerStart )
		{
			auto player = GetFirstLocalPlayerController();
			if ( AHackNSlashAthonCharacter* pawn = Cast<AHackNSlashAthonCharacter>(player->GetPawn()))
			{
				SpawnLocation.Z += 100;
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
			SpawnLocation.X += 50.0;
			SpawnLocation.Y += 50.0;
			// Spawn the pawn immediately, but delay spawning/possessing the controller.
			AHSAAICharacter* AICharacter_DTO = SpawnConfigItem->SpawneableActor->GetDefaultObject<AHSAAICharacter>();
			TSubclassOf<AAIController> ControllerClass = nullptr;
			if (AICharacter_DTO)
			{
				ControllerClass = AICharacter_DTO->AIControllerClass;
			}

			AHSAAICharacter* SpawnedActor = CurrentWorld->SpawnActor<AHSAAICharacter>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params);
			if (SpawnedActor == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("PopulateLevel: Failed to spawn enemy pawn at index %d."), i);
				continue;
			}

			// Add pawn to spawned list now (we have a pawn). Possession will occur after a delay.
			SpawnedActors.Add(SpawnedActor);

			// If controller class is valid, schedule spawn + possess after AIStartDelay
			if (ControllerClass)
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, FName("SpawnAndPossessController"), SpawnedActor, ControllerClass);

				FTimerHandle Handle;
				// Schedule with the configured AIStartDelay (can be set in editor/class defaults)
				CurrentWorld->GetTimerManager().SetTimer(Handle, TimerDel, AIStartDelay, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PopulateLevel: Enemy pawn class has no AIControllerClass specified (index %d)."), i);
			}
			
		}
		else if (EntityType == EHSAEntityType::Hole)
		{
			CollisionType = ECollisionEnabled::NoCollision;
			GroundVisibility = false;
			auto actor = CurrentWorld->SpawnActor<AActor>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params);
			SpawnedActors.Add(actor);
			if (actor == nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("PopulateLevel: Hole not created (index %d)."), i);

			}
		}
		else
		{
			//spawns which don't need any other configuration other than spawning
			SpawnedActors.Add(CurrentWorld->SpawnActor<AActor>(SpawnConfigItem->SpawneableActor, SpawnLocation, FRotator::ZeroRotator, Params));
		}

		staticMeshComp->SetVisibility(GroundVisibility);
		staticMeshComp->SetCollisionEnabled(CollisionType);
		if ( boxCollision )
		{
			boxCollision->SetCollisionEnabled(CollisionType);
		}
	}
}

void UHSAGameInstance::SpawnAndPossessController(AHSAAICharacter* Pawn, TSubclassOf<AAIController> ControllerClass)
{
	if (!Pawn || Pawn->IsPendingKillPending())
	{
		return;
	}

	UWorld* World = Pawn->GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAIController* SpawnedController = World->SpawnActor<AAIController>(ControllerClass, Pawn->GetActorLocation(), Pawn->GetActorRotation(), Params);
	if (!SpawnedController)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnAndPossessController: Failed to spawn controller for pawn %s."), *GetNameSafe(Pawn));
		Pawn->Destroy(); // Clean up the pawn if we can't control it
		return;
	}

	SpawnedController->Possess(Pawn);
}

void UHSAGameInstance::CleanLevel()
{
	for (auto Actor : SpawnedActors)
	{
		if (Actor == nullptr)
			continue;

		Actor->Destroy();
	}
	SpawnedActors.Empty();
}
