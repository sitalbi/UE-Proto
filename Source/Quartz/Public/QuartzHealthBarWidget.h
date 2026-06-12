// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuartzHealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUARTZ_API UQuartzHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void SetHealthPercent(float Percent);
};
