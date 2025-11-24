// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuartzInteractableComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UQuartzInteractableComponent::UQuartzInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InitSphereRadius(100.f);
	SetGenerateOverlapEvents(true);
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	bIsOneTimeInteractable = true;
}


// Called when the game starts
void UQuartzInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UQuartzInteractableComponent::OnRegister()
{
	Super::OnRegister();
	
}

void UQuartzInteractableComponent::DisableInteraction()
{
	SetGenerateOverlapEvents(false);
}

bool UQuartzInteractableComponent::IsOneTimeInteractable() const
{
	return bIsOneTimeInteractable;
}



