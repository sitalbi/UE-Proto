// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuartzInteractableInterface.h"
#include "Components/SphereComponent.h"
#include "QuartzInteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUARTZ_API UQuartzInteractableComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuartzInteractableComponent();

protected:

	UPROPERTY(EditAnywhere)
	float InteractionRadius;

	UPROPERTY(EditAnywhere)
	bool bIsOneTimeInteractable;

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:	

	void DisableInteraction();

	bool IsOneTimeInteractable() const;
};
