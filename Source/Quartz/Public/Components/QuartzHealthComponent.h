// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Components/WidgetComponent.h>
#include <GameplayAbilitySystem/AttributeSets/QuartzHealthAttributeSet.h>
#include "QuartzHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUARTZ_API UQuartzHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuartzHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateHealthBar();

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

protected:

	UPROPERTY()
	TObjectPtr<UWidgetComponent> HealthBarWidget;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UQuartzHealthAttributeSet> HealthSet;
};
