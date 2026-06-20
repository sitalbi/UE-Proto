// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/QuartzCharacter.h"
#include "GameplayAbilitySystem/Abilities/QuartzDash.h"

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
}

void AQuartzCharacter::InitDefaultAbilities()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);

        for (auto AbilityClass : DefaultAbilities)
        {
            if (AbilityClass)
            {
                AbilitySystemComponent->GiveAbility(
                    FGameplayAbilitySpec(AbilityClass, 1)
                );
            }
        }
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


