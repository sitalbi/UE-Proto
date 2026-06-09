// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/Attacks/GA_QuartzBaseAttack.h"
#include "QuartzGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_QuartzBaseAttack::UGA_QuartzBaseAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;


    ActivationBlockedTags.AddTag(QuartzTags::State_Attacking);
    ActivationBlockedTags.AddTag(QuartzTags::State_Stunned);

    ActivationOwnedTags.AddTag(QuartzTags::State_Attacking);
}

void UGA_QuartzBaseAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (!Character || !AttackMontage)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }


    UE_LOG(LogTemp, Warning, TEXT("Attack Started"));

    // Get desired attack direction
    FVector WorldDir = Character->GetLastMovementInputVector();

    if (WorldDir.IsNearlyZero())
    {
        WorldDir = Character->GetActorForwardVector();
    }

    // Flatten to ground plane
    WorldDir.Z = 0.f;
    WorldDir.Normalize();

    // Convert to facing rotation
    FRotator DesiredRotation = WorldDir.Rotation();

    // Instantly face that direction
    Character->SetActorRotation(DesiredRotation);

    // Play montage
    if (AttackMontage)
    {
        UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                this,
                NAME_None,
                AttackMontage,
                1.0f
            );

        Task->OnCompleted.AddDynamic(this, &UGA_QuartzBaseAttack::OnMontageFinished);
        Task->OnInterrupted.AddDynamic(this, &UGA_QuartzBaseAttack::OnMontageFinished);
        Task->OnCancelled.AddDynamic(this, &UGA_QuartzBaseAttack::OnMontageFinished);

        Task->ReadyForActivation();
    }
    else
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
    }
}

void UGA_QuartzBaseAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

    UE_LOG(LogTemp, Warning, TEXT("Attack Finished"));
}

void UGA_QuartzBaseAttack::OnMontageFinished()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
