// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Abilities/GA_QuartzDash.h"
#include "QuartzGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Abilities/Tasks/AbilityTask_PlayMontageAndWait.h>

UGA_QuartzDash::UGA_QuartzDash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	
	AbilityTags.AddTag(QuartzTags::Ability_Dash);
}

void UGA_QuartzDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("ActivateAbility"));

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

    FVector LocalDir = Character->GetActorRotation().UnrotateVector(WorldDir);
    LocalDir.Z = 0.0f;
    LocalDir.Normalize();

    // Choose section based on direction
    FName SectionName = NAME_None;

    float X = LocalDir.X;
    float Y = LocalDir.Y;

    if (X > 0.5f && FMath::Abs(Y) < 0.4f)                     SectionName = "Dash_F";   
    else if (X < -0.5f && FMath::Abs(Y) < 0.4f)               SectionName = "Dash_B";   
    else if (Y > 0.5f && FMath::Abs(X) < 0.4f)                SectionName = "Dash_R";   
    else if (Y < -0.5f && FMath::Abs(X) < 0.4f)               SectionName = "Dash_L";   

    else if (X > 0.5f && Y > 0.5f)                            SectionName = "Dash_FR";  
    else if (X > 0.5f && Y < -0.5f)                           SectionName = "Dash_FL";  
    else if (X < -0.5f && Y > 0.5f)                           SectionName = "Dash_BR";  
    else if (X < -0.5f && Y < -0.5f)                          SectionName = "Dash_BL";  

    else                                                      SectionName = "Dash_F"; 

    // Play Montage + jump to correct section
    UAbilityTask_PlayMontageAndWait* MontageTask =
        UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            DashMontage,
            1.0f,
            SectionName,
            false
        );

    MontageTask->OnCompleted.AddDynamic(this, &UGA_QuartzDash::OnDashFinished);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_QuartzDash::OnDashFinished);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_QuartzDash::OnDashFinished);
    MontageTask->ReadyForActivation();
}

void UGA_QuartzDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Warning, TEXT("EndAbility"));
}

void UGA_QuartzDash::OnDashFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
