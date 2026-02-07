#pragma once
#include "HSALevelGeneration.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelGenerated);

UCLASS()
class UHSALevelGeneration : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnLevelGenerated OnLevelGenerated;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	
	const TArray<int32>& GetCurrentLevelMap() const { return CurrentLevelMap; }
	void GenerateLevel();

private:
	TArray<int32> CurrentLevelMap;
};
