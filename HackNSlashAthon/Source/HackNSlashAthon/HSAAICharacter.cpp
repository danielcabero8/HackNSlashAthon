// Fill out your copyright notice in the Description page of Project Settings.


#include "HSAAICharacter.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AHSAAICharacter::AHSAAICharacter() : Super()
{

	if (auto AICapsuleComponent = GetCapsuleComponent()) {
		auto CapsuleRelativeLocation = AICapsuleComponent->GetRelativeLocation();

		CapsuleRelativeLocation.Z = -AICapsuleComponent->GetScaledCapsuleHalfHeight();
		AICapsuleComponent->SetRelativeLocation(CapsuleRelativeLocation);
	}
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHSAAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHSAAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHSAAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FGenericTeamId AHSAAICharacter::GetGenericTeamId() const
{
	return FGenericTeamId(TeamId);
}

