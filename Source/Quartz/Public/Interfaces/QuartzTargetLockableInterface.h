// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuartzTargetLockableInterface.generated.h"


UINTERFACE(MinimalAPI)
class UQuartzTargetLockableInterface : public UInterface
{
	GENERATED_BODY()
};

class IQuartzTargetLockableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Target Lock")
	bool CanBeLockedOn() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Target Lock")
	FVector GetLockOnLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Target Lock")
	float GetLockOnDistance() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Target Lock")
	float GetLockOutDistance() const;
};