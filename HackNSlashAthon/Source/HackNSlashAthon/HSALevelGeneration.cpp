#include "HSALevelGeneration.h"

#include "HSAGameInstance.h"

void UHSALevelGeneration::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHSALevelGeneration::GenerateLevel()
{
	UHSAGameInstance* gameInstance = Cast<UHSAGameInstance>(GetWorld()->GetGameInstance());
	if ( !ensure(gameInstance))
	{
		return;
	}
	
	const int32 mapSize = gameInstance->Columns * gameInstance->Rows;
	CurrentLevelMap = TArray<int32>();
	CurrentLevelMap.Reserve(mapSize);
	
	for ( int i = 0; i < mapSize; i++ )
	{
		const int value = FMath::RandRange(0, 1);
		CurrentLevelMap.Add(value);
	}
	
	OnLevelGenerated.Broadcast();
}
