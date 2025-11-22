// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuartzTargetLockComponent.generated.h"

class UInputAction;
struct FInputActionValue;
class AQuartzCharacter;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUARTZ_API UQuartzTargetLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuartzTargetLockComponent();

	enum class EDirection
	{
		Left,
		Right
	};


protected:

	virtual void BeginPlay() override;

	void LockOnTarget(const FInputActionValue& Value);

	bool bIsLockedOn = false;

	// TODO: make lockOnDiestance a variable in Enemy class and so dependent on the enemy type
	UPROPERTY(EditAnywhere, Category = "Lock variables")
	float lockOnDistance = 1000.0f;


	UPROPERTY(EditAnywhere, Category = "Lock variables")
	float interpolationSpeed = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Lock variables")
	float distanceFactor = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Lock variables")
	float clampAngle = 35.0f;

	UPROPERTY(EditAnywhere, Category = "Lock variables")
	TSubclassOf<AActor>lockOnClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AQuartzCharacter* OwningCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* targetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* lockWidget; // TODO: Replace lockWidget logic with the same widget logic as InteractionComponent in blueprint

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D lockWidgetViewportSizeOffset = FVector2D(50, 50);

	/** Target Lock Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TargetLockAction;

	/** Switch target Lock Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchTargetLockAction;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetTargetLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> LockWidgetClass;

	FTimerHandle TargetLockTimerHandle;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsLockedOn() const { return bIsLockedOn; }

	void BindInput(class UEnhancedInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Lock Events")
	void OnLockTarget();
	UFUNCTION(BlueprintImplementableEvent, Category = "Lock Events")
	void OnUnlockTarget();

protected:

	TArray<AActor*> TraceForTarget();

	AActor* GetTargetActor(TArray<AActor*> actors);
	AActor* GetTargetActorSwitch(TArray<AActor*> actors, EDirection direction);

	void SwitchTargetLock(const FInputActionValue& Value);

	void UpdateTargetLock();

private:

	void SetLockTimer(bool IsLocked);
	void ChangeTargetActor(AActor* newTarget);

	FRotator GetLockOnRotation();

};