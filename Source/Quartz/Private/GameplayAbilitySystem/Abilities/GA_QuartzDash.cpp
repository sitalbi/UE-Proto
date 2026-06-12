// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/GA_QuartzDash.h"
#include "QuartzGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>
#include "AbilitySystemComponent.h"

UGA_QuartzDash::UGA_QuartzDash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	
	AbilityTags.AddTag(QuartzTags::Ability_Dash); 
	AbilityTags.AddTag(QuartzTags::Input_Dash); 

    ActivationOwnedTags.AddTag(QuartzTags::State_Dashing);
}

void UGA_QuartzDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (!Character || !DashMontage)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    FVector WorldDir = Character->GetLastMovementInputVector();

    if (WorldDir.IsNearlyZero())
    {
        WorldDir = Character->GetActorForwardVector();
    }
    WorldDir.Z = 0.f;
    WorldDir.Normalize();

    FVector LocalDir = Character->GetActorRotation().UnrotateVector(WorldDir);
    LocalDir.Z = 0.0f;
    LocalDir.Normalize();

    float MaxSpeed = Character->GetCharacterMovement()->GetMaxSpeed();

    UAbilityTask_ApplyRootMotionConstantForce* DashTask =
        UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
            this,
            NAME_None,
            WorldDir,
            DashStrength,
            DashDuration,
            false,
            nullptr,
            ERootMotionFinishVelocityMode::ClampVelocity,
            FVector::ZeroVector,
            MaxSpeed,
            false
        );

    DashTask->OnFinish.AddDynamic(this, &UGA_QuartzDash::OnDashFinished);
    DashTask->ReadyForActivation();

    // Choose section based on direction
    FName SectionName = NAME_None;

    float X = LocalDir.X;
    float Y = LocalDir.Y;

    if (X > 0.5f && FMath::Abs(Y) < 0.4f)          SectionName = "Dash_F";   
    else if (X < -0.5f && FMath::Abs(Y) < 0.4f)    SectionName = "Dash_B";   
    else if (Y > 0.5f && FMath::Abs(X) < 0.4f)     SectionName = "Dash_R";   
    else if (Y < -0.5f && FMath::Abs(X) < 0.4f)    SectionName = "Dash_L";   

    else if (X > 0.5f && Y > 0.5f)                 SectionName = "Dash_FR";  
    else if (X > 0.5f && Y < -0.5f)                SectionName = "Dash_FL";  
    else if (X < -0.5f && Y > 0.5f)                SectionName = "Dash_BR";  
    else if (X < -0.5f && Y < -0.5f)               SectionName = "Dash_BL";  

    else                                           SectionName = "Dash_F"; 

    const bool bWeaponEquipped = GetAbilitySystemComponentFromActorInfo() && GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(QuartzTags::State_Weapon_Equipped);

    UAnimMontage* Montage = bWeaponEquipped ? DashCombatMontage : DashMontage;
    // Play Montage + jump to correct section
    if (Montage)
    {
        // Adjust the montage playrate to the dash duration
        float SectionLength = Montage->GetSectionLength(DashMontage->GetSectionIndex(SectionName));
        float PlayRate = SectionLength / DashDuration;

        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            Montage,
            PlayRate,
            SectionName,
            true
        )->ReadyForActivation();
    }
    
}

void UGA_QuartzDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_QuartzDash::OnDashFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
