// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/Characters/QuartzCharacterBase.h"

AQuartzCharacterBase::AQuartzCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

}


void AQuartzCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AQuartzCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AQuartzCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuartzCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AQuartzCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

