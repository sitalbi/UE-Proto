// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuartzInteractableInterface.h"
#include "QuartzItemChest.generated.h"

class USphereComponent;

UCLASS()
class QUARTZ_API AQuartzItemChest : public AActor, public IQuartzInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AQuartzItemChest();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	float OpenPitchValue;

	void BeginPlay() override;

public:
	bool Interact_Implementation(APawn* InstigatorPawn);


	// Implement in the BP owner
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnOwnerInteracted(APawn* InstigatorPawn);
};
