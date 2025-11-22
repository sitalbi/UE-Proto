// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuartzEnemy.generated.h"

UCLASS()
class QUARTZ_API AQuartzEnemy : public APawn
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


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsDead() const;

	float GetLockOutDistance() const { return LockOutDistance; }
};
