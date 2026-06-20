// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "QuartzDash.generated.h"

/**
 * 
 */
UCLASS()
class QUARTZ_API UQuartzDash : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UQuartzDash();

protected:

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
	void OnDashFinished();




public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* DashCombatMontage;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashStrength = 1800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float BrakingFrictionDuringDash = 0.f;
};
