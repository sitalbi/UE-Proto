// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuartzInteractableInterface.h"
#include "Components/QuartzInteractableComponent.h"

bool IQuartzInteractableInterface::isOneTimeInteractable()
{
	// get the interactable component from the actor implementing this interface
	AActor* Actor = Cast<AActor>(this);
	if (Actor)
	{
		UQuartzInteractableComponent* InteractableComponent = Actor->FindComponentByClass<UQuartzInteractableComponent>();
		if (InteractableComponent)
		{
			return InteractableComponent->IsOneTimeInteractable();
		}
	}
	return false;
}

void IQuartzInteractableInterface::DisableInteraction()
{
	// get the interactable component from the actor implementing this interface
	AActor* Actor = Cast<AActor>(this);
	if (Actor)
	{
		UQuartzInteractableComponent* InteractableComponent = Actor->FindComponentByClass<UQuartzInteractableComponent>();
		if (InteractableComponent)
		{
			InteractableComponent->DisableInteraction();
		}
	}
}
