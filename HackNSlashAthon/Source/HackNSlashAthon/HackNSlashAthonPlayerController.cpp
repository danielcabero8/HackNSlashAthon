// Copyright Epic Games, Inc. All Rights Reserved.

#include "HackNSlashAthonPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "HackNSlashAthonCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AHackNSlashAthonPlayerController::AHackNSlashAthonPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	bIsTouch = false;
}

void AHackNSlashAthonPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AHackNSlashAthonPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AHackNSlashAthonPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AHackNSlashAthonPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AHackNSlashAthonPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AHackNSlashAthonPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AHackNSlashAthonPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AHackNSlashAthonPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AHackNSlashAthonPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AHackNSlashAthonPlayerController::OnTouchReleased);
	
	
		// New: Bind Move (Axis2D) to AddMovementInput
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHackNSlashAthonPlayerController::OnMove);
		}

		// New: Bind Jump action (Space) - start/stop
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AHackNSlashAthonPlayerController::OnJumpStarted);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHackNSlashAthonPlayerController::OnJumpStopped);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &AHackNSlashAthonPlayerController::OnJumpStopped);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHackNSlashAthonPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AHackNSlashAthonPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AHackNSlashAthonPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AHackNSlashAthonPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AHackNSlashAthonPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}


void AHackNSlashAthonPlayerController::OnMove(const FInputActionValue& Value)
{
	// Expecting an Axis2D value (X = Right, Y = Forward)
	const FVector MovementVector = Value.Get<FVector>();
	UE_LOG(LogTemplateCharacter, Display, TEXT("OnMove called: X=%f Y=%f"), MovementVector.X, MovementVector.Y);

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || MovementVector.IsNearlyZero())
	{
		return;
	}

	// Convert input to world direction based on controller rotation (keeps movement relative to camera/control orientation)
	const FRotator ControlRot = GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X); // forward
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);   // right

	// MovementVector.X -> Right (+1 = D), MovementVector.Y -> Forward (+1 = W)
	ControlledPawn->AddMovementInput(Forward, MovementVector.Y);
	ControlledPawn->AddMovementInput(Right, MovementVector.X);
}

void AHackNSlashAthonPlayerController::OnJumpStarted()
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		Char->Jump();
	}
}

void AHackNSlashAthonPlayerController::OnJumpStopped()
{
	if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
	{
		Char->StopJumping();
	}
}