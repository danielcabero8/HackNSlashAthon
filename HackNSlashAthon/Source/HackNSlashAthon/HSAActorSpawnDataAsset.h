// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HSAActorSpawnDataAsset.generated.h"


USTRUCT()
struct FSpawnConfigurationDataAssetItem {
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly)
	uint32 ActorId;

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
	TArray<FSpawnConfigurationDataAssetItem> SpawnConfig;
	
};
