#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/QuartzPlayerCharacter.h"
#include "QuartzTargetLockComponent.generated.h"


class UInputAction;
class UEnhancedInputComponent;
class UUserWidget;

struct FInputActionValue;


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BindInput(UEnhancedInputComponent* PlayerInputComponent);

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsLockedOn() const
	{
		return bIsLockedOn;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Lock")
	FVector GetTargetLocation() const;


protected:
	virtual void BeginPlay() override;

	void LockOnTarget(const FInputActionValue& Value);
	void SwitchTargetLock(const FInputActionValue& Value);

	TArray<AActor*> TraceForTarget();

	AActor* GetTargetActor(const TArray<AActor*>& Actors);
	AActor* GetTargetActorSwitch(const TArray<AActor*>& Actors, EDirection Direction);

	void UpdateTargetLock(float DeltaTime);

	bool IsLockableTarget(AActor* Actor) const;

	FVector GetLockableLocation(AActor* Actor) const;

	float GetLockableLockOnDistance(AActor* Actor) const;

	float GetLockableLockOutDistance(AActor* Actor) const;


	/*
	 * Blueprint events
	 */

	UFUNCTION(BlueprintImplementableEvent, Category = "Lock Events")
	void OnLockTarget();

	UFUNCTION(BlueprintImplementableEvent, Category = "Lock Events")
	void OnUnlockTarget();

	UFUNCTION(BlueprintImplementableEvent, Category = "Lock Events")
	void OnUpdateLockTargetPosition(FVector2D ScreenPosition);

	UFUNCTION(BlueprintImplementableEvent, Category = "Lock Events")
	void OnUpdateTargetLock();


	/*
	 * Lock state
	 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bIsLockedOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	TObjectPtr<AQuartzPlayerCharacter> OwningCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	TObjectPtr<AActor> TargetActor = nullptr;


	/*
	 * Lock configuration
	 */

	UPROPERTY(EditAnywhere, Category = "Lock Variables")
	float MaxLockSearchDistance = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Lock Variables")
	float interpolationSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Lock Variables")
	float distanceFactor = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Lock Variables")
	float clampAngle = 35.0f;


	/*
	 * Input
	 */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TargetLockAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SwitchTargetLockAction = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> lockWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FVector2D lockWidgetViewportSizeOffset = FVector2D(50.0f, 50.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> LockWidgetClass;


private:
	void ChangeTargetActor(AActor* NewTarget);

	FRotator GetLockOnRotation() const;
};