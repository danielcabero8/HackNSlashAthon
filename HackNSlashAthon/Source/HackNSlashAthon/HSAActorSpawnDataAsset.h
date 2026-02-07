// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HSAGameLoop.h"
#include "HSAActorSpawnDataAsset.generated.h"


USTRUCT()
struct FHSASpawnConfigurationDataAssetItem {
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly)
	EHSAEntityType TypeID = EHSAEntityType::Floor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> SpawneableActor;

};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class HACKNSLASHATHON_API UHSAActorSpawnDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FHSASpawnConfigurationDataAssetItem> SpawnConfig;
	
};
