// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HSAGameLoop.h"
#include "HSAActorSpawnDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FHSASpawnConfigurationDataAssetItem {
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EHSAEntityType TypeID = EHSAEntityType::Floor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> SpawneableActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ActorOffsetZ = 0.0;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract)	
class HACKNSLASHATHON_API UHSAActorSpawnDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHSASpawnConfigurationDataAssetItem> SpawnConfig;
	
};
