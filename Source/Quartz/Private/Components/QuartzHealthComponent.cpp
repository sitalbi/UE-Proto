// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuartzHealthComponent.h"
#include <AbilitySystemInterface.h>
#include <QuartzHealthBarWidget.h>

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

    UpdateHealthBar();
}

void UQuartzHealthComponent::UpdateHealthBar()
{
    if (!HealthBarWidget || !ASC)
        return;

    UUserWidget* UserWidget = HealthBarWidget->GetUserWidgetObject();
    UQuartzHealthBarWidget* HealthWidget = Cast<UQuartzHealthBarWidget>(UserWidget);
    if (!HealthWidget)
        return;

    const float Health = ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetHealthAttribute());

    const float MaxHealth = ASC->GetNumericAttribute(UQuartzHealthAttributeSet::GetMaxHealthAttribute());

    const float Percent = MaxHealth > 0.f
        ? FMath::Clamp(Health / MaxHealth, 0.f, 1.f)
        : 0.f;

    HealthWidget->SetHealthPercent(Percent);
}

void UQuartzHealthComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    UpdateHealthBar();
}

void UQuartzHealthComponent::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    UpdateHealthBar();
}


// Called every frame
void UQuartzHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

