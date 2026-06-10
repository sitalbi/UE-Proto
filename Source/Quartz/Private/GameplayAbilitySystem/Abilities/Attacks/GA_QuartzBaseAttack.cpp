// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/Attacks/GA_QuartzBaseAttack.h"
#include "QuartzGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>

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
        CurrentComboIndex = 0;
        const FName InitialSection = AnimMontageComboSections.IsValidIndex(CurrentComboIndex) ? AnimMontageComboSections[CurrentComboIndex] : NAME_None;

        if (ComboInputEventTag.IsValid())
        {
            UAbilityTask_WaitGameplayEvent* InputTask =
                UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                    this,
                    ComboInputEventTag
                );

            InputTask->EventReceived.AddDynamic(
                this,
                &UGA_QuartzBaseAttack::OnComboInputEvent
            );

            InputTask->ReadyForActivation();
        }

        {
            UAbilityTask_WaitGameplayEvent* Task =
                UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                    this,
                    QuartzTags::Event_Combo_Window_Open
                );

            Task->EventReceived.AddDynamic(
                this,
                &UGA_QuartzBaseAttack::OnComboWindowOpenEvent
            );

            Task->ReadyForActivation();
        }

        {
            UAbilityTask_WaitGameplayEvent* Task =
                UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                    this,
                    QuartzTags::Event_Combo_Window_Close
                );

            Task->EventReceived.AddDynamic(
                this,
                &UGA_QuartzBaseAttack::OnComboWindowCloseEvent
            );

            Task->ReadyForActivation();
        }

        {
            UAbilityTask_WaitGameplayEvent* Task =
                UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                    this,
                    QuartzTags::Event_Combo_Commit
                );

            Task->EventReceived.AddDynamic(
                this,
                &UGA_QuartzBaseAttack::OnComboCommitEvent
            );

            Task->ReadyForActivation();
        }

        UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                this,
                NAME_None,
                AttackMontage,
                1.0f,
                InitialSection
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

void UGA_QuartzBaseAttack::OnComboInputEvent(FGameplayEventData Payload)
{

    UE_LOG(LogTemp, Warning, TEXT("OnComboInputEvent"));

    if (bComboWindowOpen)
    {
        bQueuedNextCombo = true;
    }
}

void UGA_QuartzBaseAttack::OnComboWindowOpenEvent(FGameplayEventData Payload)
{
    bComboWindowOpen = true;
}

void UGA_QuartzBaseAttack::OnComboWindowCloseEvent(FGameplayEventData Payload)
{
    bComboWindowOpen = false;
}

void UGA_QuartzBaseAttack::OnComboCommitEvent(FGameplayEventData Payload)
{
    if (!AttackMontage)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    const int32 NextComboIndex = CurrentComboIndex + 1;

    if (bQueuedNextCombo && AnimMontageComboSections.IsValidIndex(NextComboIndex))
    {
        CurrentComboIndex = NextComboIndex;
        bQueuedNextCombo = false;
        bComboWindowOpen = false;

        ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
        if (!Character || !Character->GetMesh())
        {
            return;
        }

        UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
        if (!AnimInstance)
        {
            return;
        }

        AnimInstance->Montage_JumpToSection(
            AnimMontageComboSections[CurrentComboIndex],
            AttackMontage
        );

        return;
    }
}
