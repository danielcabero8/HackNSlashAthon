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

	// override AGameModeBase
	void PostInitializeComponents() override;

	UFUNCTION(BlueprintPure)
	const UHSAActorSpawnDataAsset* GetActorSpawnDataAsset() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHSAActorSpawnDataAsset> ActorSpawnDataAssetClass = nullptr;
	
};

inline const UHSAActorSpawnDataAsset*  AHackNSlashAthonGameMode::GetActorSpawnDataAsset() const
{
	return ActorSpawnDataAssetClass->GetDefaultObject<UHSAActorSpawnDataAsset>();
}

