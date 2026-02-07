// Copyright Epic Games, Inc. All Rights Reserved.

#include "HackNSlashAthonGameMode.h"
#include "HackNSlashAthonPlayerController.h"
#include "HackNSlashAthonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHackNSlashAthonGameMode::AHackNSlashAthonGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AHackNSlashAthonPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

void AHackNSlashAthonGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Initialize ActorSpawnDataAsset after all components are done
	if (ActorSpawnDataAssetClass != nullptr)
	{
		ActorSpawnDataAsset = NewObject<UHSAActorSpawnDataAsset>(this, ActorSpawnDataAssetClass);
	}

}