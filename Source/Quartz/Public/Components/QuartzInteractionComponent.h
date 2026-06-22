// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuartzInteractableInterface.h"
#include "QuartzInteractionComponent.generated.h"


class UInputAction;
class UWidgetComponent;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUARTZ_API UQuartzInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuartzInteractionComponent();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TScriptInterface<IQuartzInteractableInterface>> OverlappingInteractables;

	

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void Interact();

public:	
	void BindInput(class UEnhancedInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractableAdded(const TScriptInterface<IQuartzInteractableInterface>& NewInteractable);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractableRemoved(const TScriptInterface<IQuartzInteractableInterface>& RemovedInteractable);



};
