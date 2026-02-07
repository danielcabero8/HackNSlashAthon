// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "HSAAICharacter.generated.h"

UCLASS()
class HACKNSLASHATHON_API AHSAAICharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHSAAICharacter();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;
	
public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// IGenericTeamAgentInterface
	FGenericTeamId GetGenericTeamId() const override;

	// Team id exposed to editor/blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	uint8 TeamId = 1;
};
