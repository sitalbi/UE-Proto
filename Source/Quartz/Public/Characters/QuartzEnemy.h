// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem/Characters/QuartzCharacterBase.h"
#include "QuartzEnemy.generated.h"

UCLASS()
class QUARTZ_API AQuartzEnemy : public AQuartzCharacterBase
{
	GENERATED_BODY()

public:
	AQuartzEnemy();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LockOutDistance = 1000.0f;

public:	

	virtual void Tick(float DeltaTime) override;

	bool IsDead() const;

	float GetLockOutDistance() const { return LockOutDistance; }
};
