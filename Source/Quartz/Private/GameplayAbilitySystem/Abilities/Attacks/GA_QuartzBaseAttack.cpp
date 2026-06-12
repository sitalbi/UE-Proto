// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/Attacks/GA_QuartzBaseAttack.h"
#include "QuartzGameplayTags.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Abilities/Tasks/AbilityTask_WaitGameplayEvent.h>
#include <AbilitySystemInterface.h>
#include <AbilitySystemComponent.h>
#include <Characters/QuartzCharacter.h>

UGA_QuartzBaseAttack::UGA_QuartzBaseAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    ActivationBlockedTags.AddTag(QuartzTags::State_Attacking);
    ActivationBlockedTags.AddTag(QuartzTags::State_Stunned);

    ActivationOwnedTags.AddTag(QuartzTags::State_Attacking);
}

void UGA_QuartzBaseAttack::ApplyDamageToActor(AActor* TargetActor)
{
    UE_LOG(LogTemp, Warning, TEXT("ApplyDamageToActor called. Target=%s"), *GetNameSafe(TargetActor));

    AQuartzCharacter* Character = Cast<AQuartzCharacter>(GetAvatarActorFromActorInfo());
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("Damage failed: Avatar is not AQuartzCharacter"));
        return;
    }

    if (!Character->WeaponData)
    {
        UE_LOG(LogTemp, Error, TEXT("Damage failed: WeaponData is null"));
        return;
    }

    if (!Character->WeaponData->DamageEffectClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Damage failed: DamageEffectClass is null"));
        return;
    }

    IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(TargetActor);
    if (!TargetASI)
    {
        UE_LOG(LogTemp, Error, TEXT("Damage failed: Target does not implement IAbilitySystemInterface"));
        return;
    }

    UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
    if (!TargetASC)
    {
        UE_LOG(LogTemp, Error, TEXT("Damage failed: Target ASC is null"));
        return;
    }

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Character->WeaponData->DamageEffectClass, GetAbilityLevel());


    SpecHandle.Data->SetSetByCallerMagnitude(
        QuartzTags::Data_Damage,
        -Character->WeaponData->BaseDamage
    );
    
    if (!SpecHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Damage failed: SpecHandle invalid"));
        return;
    }

    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
        *SpecHandle.Data.Get(),
        TargetASC
    );

    UE_LOG(LogTemp, Warning, TEXT("Damage applied to %s using %s"), *GetNameSafe(TargetActor), *GetNameSafe(Character->WeaponData->DamageEffectClass));
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

        {
            UAbilityTask_WaitGameplayEvent* HitTask =
                UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                    this,
                    QuartzTags::Event_Attack_Hit
                );

            HitTask->EventReceived.AddDynamic(
                this,
                &UGA_QuartzBaseAttack::OnAttackHitEvent
            );

            HitTask->ReadyForActivation();
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
}

void UGA_QuartzBaseAttack::OnMontageFinished()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_QuartzBaseAttack::OnComboInputEvent(FGameplayEventData Payload)
{

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

void UGA_QuartzBaseAttack::OnAttackHitEvent(FGameplayEventData Payload)
{
    AActor* HitActor = const_cast<AActor*>(Payload.Target.Get());

    if (!HitActor)
    {
        return;
    }

    ApplyDamageToActor(HitActor);
}
