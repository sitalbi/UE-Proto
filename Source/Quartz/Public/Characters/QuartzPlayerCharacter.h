// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/QuartzInteractionComponent.h"
#include "Characters/QuartzCharacter.h"
#include <QuartzWeaponData.h>
#include "QuartzPlayerCharacter.generated.h"

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
class QUARTZ_API AQuartzPlayerCharacter : public AQuartzCharacter
{
	GENERATED_BODY()

public:
	AQuartzPlayerCharacter();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	UCameraComponent* GetFollowCamera();


	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeapon(bool bEquip);



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

	void LightAttack();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	bool Debug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UQuartzWeaponData> WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName WeaponHandSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FName WeaponSheathSocket;

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

	// Equip action (Enhanced Input System)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LightAttackAction;

	// Equip anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponMeshComp;

private:
	// Spring Arm Component
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	// Camera Component
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UQuartzTargetLockComponent> TargetLockComp;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> WeaponAbilityHandles;

	UPROPERTY()
	FGameplayTag EquipTag;

	float DefaultSpeed = 600.0f;

	bool isEquipped = false;
};
