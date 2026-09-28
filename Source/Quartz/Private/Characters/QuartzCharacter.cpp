// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/QuartzCharacter.h"
#include "GameplayAbilitySystem/Abilities/QuartzDash.h"
#include "QuartzGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

AQuartzCharacter::AQuartzCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

    HealthSet = CreateDefaultSubobject<UQuartzHealthAttributeSet>(TEXT("HealthAttributeSet"));

}


void AQuartzCharacter::BeginPlay()
{
	Super::BeginPlay();

    InitDefaultAbilities();

    // Init tags
    UpdateMovementStateTags();
}

void AQuartzCharacter::InitDefaultAbilities()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        for (auto& AbilityClass : DefaultAbilities)
        {
            if (AbilityClass)
            {
                AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
            }
        }
    }
}

void AQuartzCharacter::UpdateMovementStateTags()
{
    if (!AbilitySystemComponent) return;

    AbilitySystemComponent->RemoveLooseGameplayTag(QuartzTags::State_Movement_Grounded);
    AbilitySystemComponent->RemoveLooseGameplayTag(QuartzTags::State_Movement_Air);

    const EMovementMode Mode = GetCharacterMovement()->MovementMode;

    if (Mode == MOVE_Walking || Mode == MOVE_NavWalking)
    {
        AbilitySystemComponent->AddLooseGameplayTag(QuartzTags::State_Movement_Grounded);
    }
    else if (Mode == MOVE_Falling || Mode == MOVE_Flying)
    {
        AbilitySystemComponent->AddLooseGameplayTag(QuartzTags::State_Movement_Air);
    }
}

void AQuartzCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AQuartzCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AQuartzCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
    UpdateMovementStateTags();
}


