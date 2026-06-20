// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include "QuartzHitReaction.generated.h"

/**
 * 
 */
UCLASS()
class QUARTZ_API UQuartzHitReaction : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UQuartzHitReaction();
	
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
	void OnMontageFinished();

	UFUNCTION()
	void SetupHitReactEventListener();

	UFUNCTION()
	void PlayHitReactionFromEvent(const FGameplayEventData& Payload);

	UFUNCTION()
	void OnHitReactEvent(FGameplayEventData Payload);


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	UAnimMontage* HitMontage;

protected:

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
