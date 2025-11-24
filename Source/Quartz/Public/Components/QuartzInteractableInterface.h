// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QuartzInteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UQuartzInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class QUARTZ_API IQuartzInteractableInterface
{
	GENERATED_BODY()

public:
	
	
	UFUNCTION(BlueprintNativeEvent)
	bool Interact(APawn* InstigatorPawn); 

	bool isOneTimeInteractable();

	void DisableInteraction();

};
