// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "QuartzBaseAttack.generated.h"

/**
 * 
 */
UCLASS()
class QUARTZ_API UQuartzBaseAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UQuartzBaseAttack();

protected:

	void ApplyDamageToActor(AActor* TargetActor);

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnComboInputEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowOpenEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowCloseEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboCommitEvent(FGameplayEventData Payload);

	UFUNCTION()
	void OnAttackHitEvent(FGameplayEventData Payload);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	FGameplayTag ComboInputEventTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<FName> AnimMontageComboSections;

protected:
	bool bComboWindowOpen = false;
	bool bQueuedNextCombo = false;

	int CurrentComboIndex = 0;
};
