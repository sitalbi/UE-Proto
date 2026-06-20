// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/QuartzHitReaction.h"
#include "QuartzGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include "GameFramework/Character.h"

UQuartzHitReaction::UQuartzHitReaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//bRetriggerInstancedAbility = true;

	AbilityTags.AddTag(QuartzTags::Ability_HitReact);

	ActivationOwnedTags.AddTag(QuartzTags::State_HitReact);
	ActivationBlockedTags.AddTag(QuartzTags::State_Dead);
	//ActivationBlockedTags.AddTag(QuartzTags::State_HitReact);

	//CancelAbilitiesWithTag.AddTag(QuartzTags::Ability_HitReact);
}

void UQuartzHitReaction::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	SetupHitReactEventListener();

	if (TriggerEventData)
	{
		PlayHitReactionFromEvent(*TriggerEventData);
	}
}

void UQuartzHitReaction::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	MontageTask = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQuartzHitReaction::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UQuartzHitReaction::SetupHitReactEventListener()
{
	UAbilityTask_WaitGameplayEvent* HitReactTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			QuartzTags::Event_Attack_HitReact,
			nullptr,
			false,
			false
		);

	HitReactTask->EventReceived.AddDynamic(
		this,
		&UQuartzHitReaction::OnHitReactEvent
	);

	HitReactTask->ReadyForActivation();
}

void UQuartzHitReaction::PlayHitReactionFromEvent(const FGameplayEventData& Payload)
{
	AActor* HitActor = GetAvatarActorFromActorInfo();
	AActor* Attacker = const_cast<AActor*>(Payload.Instigator.Get());

	if (!HitActor || !Attacker || !HitMontage)
	{
		return;
	}

	FVector Direction = Attacker->GetActorLocation() - HitActor->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();

	FVector LocalDir = HitActor->GetActorRotation().UnrotateVector(Direction);

	FName SectionName = NAME_None;

	if (LocalDir.X > 0.5f)        SectionName = TEXT("Hit_F");
	else if (LocalDir.X < -0.5f)  SectionName = TEXT("Hit_B");
	else if (LocalDir.Y > 0.5f)   SectionName = TEXT("Hit_R");
	else                          SectionName = TEXT("Hit_L");

	ACharacter* Character = Cast<ACharacter>(HitActor);
	if (!Character || !Character->GetMesh()) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	// If already playing, restart/jump section
	if (AnimInstance->Montage_IsPlaying(HitMontage))
	{
		AnimInstance->Montage_JumpToSection(SectionName, HitMontage);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			HitMontage,
			1.0f,
			SectionName,
			true
		);

	Task->OnCompleted.AddDynamic(this, &UQuartzHitReaction::OnMontageFinished);
	Task->OnInterrupted.AddDynamic(this, &UQuartzHitReaction::OnMontageFinished);
	Task->OnCancelled.AddDynamic(this, &UQuartzHitReaction::OnMontageFinished);

	Task->ReadyForActivation();
}

void UQuartzHitReaction::OnHitReactEvent(FGameplayEventData Payload)
{
	PlayHitReactionFromEvent(Payload);
}