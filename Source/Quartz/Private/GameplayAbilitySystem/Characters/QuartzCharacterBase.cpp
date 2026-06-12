// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Characters/QuartzCharacterBase.h"
#include "GameplayAbilitySystem/Abilities/GA_QuartzDash.h"

AQuartzCharacterBase::AQuartzCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

    HealthSet = CreateDefaultSubobject<UQuartzHealthAttributeSet>(TEXT("HealthAttributeSet"));

}


void AQuartzCharacterBase::BeginPlay()
{
	Super::BeginPlay();

    InitDefaultAbilities();
}

void AQuartzCharacterBase::InitDefaultAbilities()
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

void AQuartzCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AQuartzCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


