// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/QuartzInteractionComponent.h"
#include "GameFramework/Character.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include <Components/WidgetComponent.h>


// Sets default values for this component's properties
UQuartzInteractionComponent::UQuartzInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UQuartzInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (UPrimitiveComponent* OwnerCapsule = OwnerCharacter->GetCapsuleComponent())
		{
			OwnerCapsule->OnComponentBeginOverlap.AddDynamic(this, &UQuartzInteractionComponent::OnOverlapBegin);
			OwnerCapsule->OnComponentEndOverlap.AddDynamic(this, &UQuartzInteractionComponent::OnOverlapEnd);
		}
	}
}


void UQuartzInteractionComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnter"));
	TScriptInterface<IQuartzInteractableInterface> Intf(OtherActor);
	if (OtherActor && OtherActor != GetOwner() && Intf)
	{
		OverlappingInteractables.AddUnique(Intf);
		OnInteractableAdded(Intf);
	}
}

void UQuartzInteractionComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd"));
	TScriptInterface<IQuartzInteractableInterface> Intf(OtherActor);
	if (OtherActor && OtherActor != GetOwner() && Intf)
	{
		OverlappingInteractables.Remove(Intf);
		OnInteractableRemoved(Intf);
	}
}

void UQuartzInteractionComponent::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact Pressed"));
	if (OverlappingInteractables.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappingInteractables non nul"));
		TScriptInterface<IQuartzInteractableInterface> Interactable = OverlappingInteractables[0];
		bool bDisablePostInteraction = IQuartzInteractableInterface::Execute_Interact(Interactable.GetObject(), Cast<ACharacter>(GetOwner()));

		if (bDisablePostInteraction) {
			OverlappingInteractables.Remove(Interactable);
			OnInteractableRemoved(Interactable);
		}
	}
}


void UQuartzInteractionComponent::BindInput(UEnhancedInputComponent* PlayerInputComponent)
{
	if (InteractAction && PlayerInputComponent)
	{
		PlayerInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &UQuartzInteractionComponent::Interact);
	}
}






