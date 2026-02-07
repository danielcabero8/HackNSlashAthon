// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HSAActorSpawnDataAsset.h"
#include "HackNSlashAthonGameMode.generated.h"

struct FHSASpawnConfigurationDataAssetItem;

UCLASS(minimalapi)
class AHackNSlashAthonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHackNSlashAthonGameMode();

	UFUNCTION(BlueprintPure)
	const UHSAActorSpawnDataAsset* GetActorSpawnDataAsset() const;


	const FHSASpawnConfigurationDataAssetItem* GetSpawnConfiguration(EHSAEntityType EntityType) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHSAActorSpawnDataAsset> ActorSpawnDataAssetClass = nullptr;

};

inline const UHSAActorSpawnDataAsset*  AHackNSlashAthonGameMode::GetActorSpawnDataAsset() const
{
	return ActorSpawnDataAssetClass!= nullptr ? ActorSpawnDataAssetClass->GetDefaultObject<UHSAActorSpawnDataAsset>() : nullptr;
}

