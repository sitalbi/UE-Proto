#include "GameplayAbilitySystem/Abilities/QuartzHitReaction.h"

#include "QuartzGameplayTags.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

UQuartzHitReaction::UQuartzHitReaction()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(QuartzTags::Ability_HitReact);

	ActivationOwnedTags.AddTag(QuartzTags::State_HitReact);
	ActivationBlockedTags.AddTag(QuartzTags::State_Dead);

	CancelAbilitiesWithTag.AddTag(QuartzTags::Ability_Attack);
}

void UQuartzHitReaction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("HitReact ACTIVATED: %s | Event: %s"),
		*GetNameSafe(GetAvatarActorFromActorInfo()),
		TriggerEventData ? TEXT("Valid") : TEXT("Null"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// This ability requires a hit event to start a reaction.
	// Do not leave it active indefinitely if it was activated without one.
	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReact: No trigger event. Ending ability."));

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Listen for additional hits while the current reaction is active.
	SetupHitReactEventListener();

	PlayHitReactionFromEvent(*TriggerEventData);
}

void UQuartzHitReaction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("HitReact ENDED: %s | Cancelled: %s"),
		*GetNameSafe(GetAvatarActorFromActorInfo()),
		bWasCancelled ? TEXT("true") : TEXT("false"));

	MontageTask = nullptr;

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled
	);
}

void UQuartzHitReaction::OnMontageFinished()
{
	if (!IsActive())
	{
		return;
	}

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		true,
		false
	);
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
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	const AActor* Attacker = Payload.Instigator.Get();

	if (!Character || !Attacker || !HitMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReact: Invalid character, attacker, or montage."));

		OnMontageFinished();
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;

	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReact: Missing animation instance."));

		OnMontageFinished();
		return;
	}

	// Direction from the enemy toward the attacker.
	FVector Direction = Attacker->GetActorLocation() - Character->GetActorLocation();
	Direction.Z = 0.f;

	if (!Direction.Normalize())
	{
		Direction = Character->GetActorForwardVector();
		Direction.Z = 0.f;
		Direction.Normalize();
	}

	const FVector LocalDir =
		Character->GetActorRotation().UnrotateVector(Direction);

	FName SectionName = NAME_None;

	if (LocalDir.X > 0.5f)
	{
		SectionName = TEXT("Hit_F");
	}
	else if (LocalDir.X < -0.5f)
	{
		SectionName = TEXT("Hit_B");
	}
	else if (LocalDir.Y > 0.5f)
	{
		SectionName = TEXT("Hit_R");
	}
	else
	{
		SectionName = TEXT("Hit_L");
	}

	// Stop the current AI movement request before applying knockback.
	if (AController* Controller = Character->GetController())
	{
		Controller->StopMovement();
	}

	Character->GetCharacterMovement()->StopMovementImmediately();

	const float KnockbackStrength = 1500.f;

	Character->LaunchCharacter(
		-Direction * KnockbackStrength,
		true,
		false
	);

	// If the hit-reaction montage is already playing, switch to the
	// appropriate directional section without creating another montage task.
	if (AnimInstance->Montage_IsPlaying(HitMontage))
	{
		AnimInstance->Montage_JumpToSection(SectionName, HitMontage);
		return;
	}

	MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			HitMontage,
			1.0f,
			SectionName,
			true
		);

	if (!MontageTask)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReact: Failed to create montage task."));

		OnMontageFinished();
		return;
	}

	MontageTask->OnCompleted.AddDynamic(
		this,
		&UQuartzHitReaction::OnMontageFinished
	);

	MontageTask->OnInterrupted.AddDynamic(
		this,
		&UQuartzHitReaction::OnMontageFinished
	);

	MontageTask->OnCancelled.AddDynamic(
		this,
		&UQuartzHitReaction::OnMontageFinished
	);

	MontageTask->ReadyForActivation();
}

void UQuartzHitReaction::OnHitReactEvent(FGameplayEventData Payload)
{
	if (!IsActive())
	{
		return;
	}

	PlayHitReactionFromEvent(Payload);
}