// Copyright Epic Games, Inc. All Rights Reserved.

#include "excesarCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "excesar.h"
#include "Components/PointLightComponent.h"
#include "InputCoreTypes.h" // for EKeys

// Command Pattern  
struct Command
{
	virtual ~Command() = default;
	virtual void Execute(class AexcesarCharacter* Receiver) = 0;
};

struct LightOnCommand : Command
{
	virtual void Execute(AexcesarCharacter* Receiver) override
	{
		if (Receiver && Receiver->DemoLight) Receiver->DemoLight->SetVisibility(true);
	}
};

struct LightOffCommand : Command
{
	virtual void Execute(AexcesarCharacter* Receiver) override
	{
		if (Receiver && Receiver->DemoLight) Receiver->DemoLight->SetVisibility(false);
	}
};
// ------------------------------------------------------


AexcesarCharacter::AexcesarCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// tiny demo light (initially off)
	DemoLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("DemoLight"));
	DemoLight->SetupAttachment(RootComponent);
	DemoLight->SetVisibility(false);
	DemoLight->SetIntensity(5000.f); // visible brightness


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AexcesarCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AexcesarCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AexcesarCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AexcesarCharacter::Look);
	}
	else
	{
		UE_LOG(Logexcesar, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	// ---- Simple key bindings for the light demo (works regardless of Enhanced Input) ----
	if (PlayerInputComponent)
	{
		// Press L -> turn demo light ON
		PlayerInputComponent->BindKey(EKeys::L, IE_Pressed, this, &AexcesarCharacter::DoLightOnCmd);

		// Press O -> turn demo light OFF
		PlayerInputComponent->BindKey(EKeys::O, IE_Pressed, this, &AexcesarCharacter::DoLightOffCmd);
	}
	// --------------------------------------------------------------------------------------
}

void AexcesarCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AexcesarCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AexcesarCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AexcesarCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AexcesarCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AexcesarCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AexcesarCharacter::DoLightOnCmd()
{
	LightOnCommand Cmd;
	Cmd.Execute(this);
}

void AexcesarCharacter::DoLightOffCmd()
{
	LightOffCommand Cmd;
	Cmd.Execute(this);
}
