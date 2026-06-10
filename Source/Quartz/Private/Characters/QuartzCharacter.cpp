#include "Characters/QuartzCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/QuartzTargetLockComponent.h"
#include "GameplayAbilitySystem/Abilities/GA_QuartzDash.h"
#include "QuartzGameplayTags.h"

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

	WeaponMeshComp = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("WeaponMesh")));

	if (!WeaponMeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponMeshComp not found"));
	}

	EquipTag = QuartzTags::State_Weapon_Equipped;
}

void AQuartzCharacter::Move(const FInputActionValue& Value)
{
	if (AbilitySystemComponent &&
		AbilitySystemComponent->HasMatchingGameplayTag(QuartzTags::State_Dashing))
	{
		return; // movement blocked
	}
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
	if (TargetLockComp && TargetLockComp->IsLockedOn())
	{
		return;
	}
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

void AQuartzCharacter::Dash()
{
	if (!AbilitySystemComponent)
		return;

	FGameplayTagContainer DashTagContainer;
	DashTagContainer.AddTag(QuartzTags::Input_Dash);

	AbilitySystemComponent->TryActivateAbilitiesByTag(DashTagContainer);
}

void AQuartzCharacter::EquipWeapon()
{
	FName SectionName = FName("Equip");
	bool bUnequip = AbilitySystemComponent->HasMatchingGameplayTag(EquipTag);
	if(bUnequip)
	{
		SectionName = FName("Unequip");
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->Montage_Play(EquipMontage);
	AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
}

void AQuartzCharacter::LightAttack()
{
	if (!AbilitySystemComponent)
		return;

	FGameplayEventData EventInputLightAttack;
	EventInputLightAttack.EventTag = QuartzTags::Event_Input_LightAttack;
	EventInputLightAttack.Instigator = this;
	EventInputLightAttack.Target = this;

	AbilitySystemComponent->HandleGameplayEvent(
		QuartzTags::Event_Input_LightAttack,
		&EventInputLightAttack
	);


	FGameplayTagContainer LightAttackTags;
	LightAttackTags.AddTag(QuartzTags::Input_Attack_Light);
	AbilitySystemComponent->TryActivateAbilitiesByTag(LightAttackTags);
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

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AQuartzCharacter::Dash);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AQuartzCharacter::EquipWeapon);

		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &AQuartzCharacter::LightAttack);

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

void AQuartzCharacter::AttachWeapon(bool bEquip)
{
	if (!AbilitySystemComponent || !WeaponData)
	{
		return;
	}

	if (!bEquip)
	{
		if (!WeaponMeshComp) return;

		/*WeaponMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		WeaponMeshComp->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponSheathSocket
		);
		WeaponMeshComp->SetRelativeTransform(FTransform::Identity);*/

		AbilitySystemComponent->RemoveLooseGameplayTag(EquipTag);
		AbilitySystemComponent->RemoveLooseGameplayTag(WeaponData->WeaponTag);

		for (FGameplayAbilitySpecHandle Handle : WeaponAbilityHandles)
		{
			AbilitySystemComponent->ClearAbility(Handle);
		}

		WeaponAbilityHandles.Empty();
	}
	else
	{
		if (!WeaponMeshComp) {

			return;
		}

		/*WeaponMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		WeaponMeshComp->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			WeaponHandSocket
		);
		FTransform transform = FTransform(FRotator(-90.0, 0, 180), FVector(-8.0,0,0));
		WeaponMeshComp->SetRelativeTransform(transform);*/

		AbilitySystemComponent->AddLooseGameplayTag(EquipTag);
		AbilitySystemComponent->AddLooseGameplayTag(WeaponData->WeaponTag);

		for (TSubclassOf<UGameplayAbility> AbilityClass : WeaponData->GrantedAbilities)
		{
			if (!AbilityClass) continue;

			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));

			WeaponAbilityHandles.Add(Handle);
		}
	}
}

