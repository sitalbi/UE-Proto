// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/QuartzInteractionComponent.h"
#include "GameplayAbilitySystem/Characters/QuartzCharacterBase.h"
#include "QuartzCharacter.generated.h"

// Forward declarations
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ASMagicProjectile;
class UAnimMontage;
class UQuartzTargetLockComponent;

UCLASS()
class QUARTZ_API AQuartzCharacter : public AQuartzCharacterBase
{
	GENERATED_BODY()

public:
	AQuartzCharacter();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	UCameraComponent* GetFollowCamera();

protected:
	virtual void BeginPlay() override;

	// Movement input handling
	void Move(const FInputActionValue& Value);

	// Look input handling
	void Look(const FInputActionValue& Value);

	// Landed override
	virtual void Landed(const FHitResult& Hit) override;

	// Jump override
	virtual void Jump() override;

	// Stop Jumping override
	virtual void StopJumping() override;

	void Dash();

	void EquipWeapon();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	bool Debug = false;

protected:
	// Input Mapping Context (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// Movement action (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	// Look action (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	// Jump action (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	// Dash action (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DashAction;

	// Equip action (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipAction;

	// Equip anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	UAnimMontage* EquipMontage;

private:
	// Spring Arm Component
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UQuartzTargetLockComponent> TargetLockComp;

	float DefaultSpeed = 600.0f;

	bool isEquipped = false;
};
