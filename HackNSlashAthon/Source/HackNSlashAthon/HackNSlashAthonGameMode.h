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
	bool UseRemoteAPIGeneration() const { return bUseRemoteAPIGeneration; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bStartDemoOnInit = false;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHSAActorSpawnDataAsset> ActorSpawnDataAssetClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	bool bUseRemoteAPIGeneration = false;
};

inline const UHSAActorSpawnDataAsset*  AHackNSlashAthonGameMode::GetActorSpawnDataAsset() const
{
	return ActorSpawnDataAssetClass!= nullptr ? ActorSpawnDataAssetClass->GetDefaultObject<UHSAActorSpawnDataAsset>() : nullptr;
}

