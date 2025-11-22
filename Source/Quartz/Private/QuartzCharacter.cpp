#include "QuartzCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "QuartzTargetLockComponent.h"

// Constructor: Sets default values for this character
AQuartzCharacter::AQuartzCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0; // Automatically possess Player 0

	// Create and attach a Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true; // Rotate spring arm with controller

	// Create and attach a Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character faces movement direction
	bUseControllerRotationYaw = false; // Disable yaw rotation based on controller

}

void AQuartzCharacter::Tick(float DeltaTime)
{
}


void AQuartzCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add input mapping context to the Enhanced Input subsystem
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set the default walk speed
	DefaultSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// Initialize the Target Lock Component
	TargetLockComp = FindComponentByClass<UQuartzTargetLockComponent>();
}

void AQuartzCharacter::Move(const FInputActionValue& Value)
{
	// Get movement input as a 2D vector
	FVector2D MovementVector = Value.Get<FVector2D>();

	// Get the controller's rotation and flatten it (ignore pitch and roll)
	FRotator ControlRot = Controller->GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// Calculate forward and right movement directions
	const FVector Forward = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::X);
	const FVector Right = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	
	AddMovementInput(Forward, MovementVector.Y);
	AddMovementInput(Right, MovementVector.X);
}

void AQuartzCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisValue.X);
	AddControllerPitchInput(LookAxisValue.Y);
}

void AQuartzCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

void AQuartzCharacter::Jump()
{
	Super::Jump();
}

void AQuartzCharacter::StopJumping()
{
	Super::StopJumping();
}


// Sets up input bindings for this character
void AQuartzCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement and look input actions
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AQuartzCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AQuartzCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AQuartzCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AQuartzCharacter::StopJumping);

		if (auto IC = FindComponentByClass<UQuartzInteractionComponent>())
		{
			IC->BindInput(EnhancedInputComponent);
		}

		if (auto TC = FindComponentByClass<UQuartzTargetLockComponent>())
		{
			TC->BindInput(EnhancedInputComponent);
		}
	}
}

UCameraComponent* AQuartzCharacter::GetFollowCamera()
{
	return CameraComp;
}

