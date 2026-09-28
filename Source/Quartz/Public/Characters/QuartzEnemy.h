// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/QuartzCharacter.h"
#include "Interfaces/QuartzTargetLockableInterface.h"
#include "QuartzEnemy.generated.h"

UCLASS()
class QUARTZ_API AQuartzEnemy : public AQuartzCharacter, public IQuartzTargetLockableInterface
{
	GENERATED_BODY()

public:
	AQuartzEnemy();

protected:
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	bool IsDead() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GameplayCue")
	void OnHitCue();

		
	UFUNCTION(BlueprintCallable)
	bool Attack();
};
