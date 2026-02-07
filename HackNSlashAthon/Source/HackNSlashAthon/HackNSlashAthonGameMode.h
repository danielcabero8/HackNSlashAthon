// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HSAActorSpawnDataAsset.h"
#include "HackNSlashAthonGameMode.generated.h"

UCLASS(minimalapi)
class AHackNSlashAthonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHackNSlashAthonGameMode();

	UFUNCTION(BlueprintPure)
	const UHSAActorSpawnDataAsset* GetActorSpawnDataAsset() const;


	TSubclassOf<AActor> GetActorToSpawn(EHSAEntityType EntityType) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHSAActorSpawnDataAsset> ActorSpawnDataAssetClass = nullptr;

};

inline const UHSAActorSpawnDataAsset*  AHackNSlashAthonGameMode::GetActorSpawnDataAsset() const
{
	return ActorSpawnDataAssetClass->GetDefaultObject<UHSAActorSpawnDataAsset>();
}

