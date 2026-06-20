// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include <GameplayAbilitySystem/QuartzAbilityInputTypes.h>
#include <GameplayAbilitySystem/AttributeSets/QuartzHealthAttributeSet.h>
#include "QuartzCharacter.generated.h"


UCLASS()
class QUARTZ_API AQuartzCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AQuartzCharacter();

protected:
	virtual void BeginPlay() override;

	void InitDefaultAbilities();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem | Health")
	TObjectPtr<UQuartzHealthAttributeSet> HealthSet;
};

