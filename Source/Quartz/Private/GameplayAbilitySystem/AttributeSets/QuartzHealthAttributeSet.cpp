// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/AttributeSets/QuartzHealthAttributeSet.h"
#include "GameplayEffectExtension.h"

UQuartzHealthAttributeSet::UQuartzHealthAttributeSet()
{
    Health = 100.f;
    MaxHealth = 100.f;
}

void UQuartzHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
}

void UQuartzHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

        UE_LOG(LogTemp, Warning, TEXT("Health changed: %.1f / %.1f"), GetHealth(), GetMaxHealth());
    }
}