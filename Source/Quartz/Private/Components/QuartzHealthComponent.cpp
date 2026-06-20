// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuartzHealthComponent.h"
#include <AbilitySystemInterface.h>
#include <QuartzHealthBarWidget.h>
#include <GameplayAbilitySystem/Abilities/QuartzHitReaction.h>
#include <QuartzGameplayTags.h>
#include "Characters/QuartzCharacter.h"

// Sets default values for this component's properties
UQuartzHealthComponent::UQuartzHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UQuartzHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner)
        return;

    HealthBarWidget = NewObject<UWidgetComponent>(Owner, TEXT("HealthBarWidget"));
    if (HealthBarWidget)
    {
        HealthBarWidget->RegisterComponent();
        HealthBarWidget->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

        HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
        HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
        HealthBarWidget->SetWidgetClass(HealthBarWidgetClass);
    }

    if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
    {
        ASC = ASI->GetAbilitySystemComponent();
    }

    if (!ASC)
        return;

    ASC->GetGameplayAttributeValueChangeDelegate(
        UQuartzHealthAttributeSet::GetHealthAttribute()
    ).AddUObject(this, &UQuartzHealthComponent::OnHealthChanged);

    ASC->GetGameplayAttributeValueChangeDelegate(
        UQuartzHealthAttributeSet::GetMaxHealthAttribute()
    ).AddUObject(this, &UQuartzHealthComponent::OnMaxHealthChanged);

    UpdateHealthBar(ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetHealthAttribute()), ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetMaxHealthAttribute()));
    UE_LOG(LogTemp, Warning, TEXT("HitReactionAbilityClass=%s"),
        *GetNameSafe(HitReactionAbilityClass));
    ASC->GiveAbility(FGameplayAbilitySpec(HitReactionAbilityClass, 1));
}

void UQuartzHealthComponent::UpdateHealthBar(const float Health, const float MaxHealth)
{
    if (!HealthBarWidget || !ASC)
        return;

    UUserWidget* UserWidget = HealthBarWidget->GetUserWidgetObject();
    UQuartzHealthBarWidget* HealthWidget = Cast<UQuartzHealthBarWidget>(UserWidget);
    if (!HealthWidget)
        return;

    

    const float Percent = MaxHealth > 0.f
        ? FMath::Clamp(Health / MaxHealth, 0.f, 1.f)
        : 0.f;

    HealthWidget->SetHealthPercent(Percent);
}

void UQuartzHealthComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    const float Health = ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetHealthAttribute());

    const float MaxHealth = ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetMaxHealthAttribute());

    UpdateHealthBar(Health, MaxHealth);

    if (Health <= 0.f && !bIsDead)
    {
        bIsDead = true;

        ASC->AddLooseGameplayTag(QuartzTags::State_Dead);

        AActor* Owner = GetOwner();
        if (!Owner) return;

        AQuartzCharacter* Character = Cast<AQuartzCharacter>(Owner);
        if (Character)
        {
            Character->OnDeath();
        }
    }

}

void UQuartzHealthComponent::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    const float Health = ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetHealthAttribute());
    const float MaxHealth = ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetMaxHealthAttribute());
    UpdateHealthBar(Health, MaxHealth);
}


// Called every frame
void UQuartzHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UQuartzHealthComponent::HideHealthBar()
{
    HealthBarWidget->SetHiddenInGame(true);
}

