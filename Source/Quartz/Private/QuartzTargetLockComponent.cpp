// Fill out your copyright notice in the Description page of Project Settings.


#include "QuartzTargetLockComponent.h"
#include "GameFramework/Character.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "QuartzEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <QuartzCharacter.h>
#include "Camera/CameraComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"

UQuartzTargetLockComponent::UQuartzTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UQuartzTargetLockComponent::BindInput(UEnhancedInputComponent* PlayerInputComponent)
{
	if (TargetLockAction && PlayerInputComponent)
	{
		PlayerInputComponent->BindAction(TargetLockAction, ETriggerEvent::Triggered, this, &UQuartzTargetLockComponent::LockOnTarget);
		PlayerInputComponent->BindAction(SwitchTargetLockAction, ETriggerEvent::Triggered, this, &UQuartzTargetLockComponent::SwitchTargetLock);
	}
}

void UQuartzTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<AQuartzCharacter>(GetOwner());
}

void UQuartzTargetLockComponent::LockOnTarget(const FInputActionValue& Value)
{
	if(OwningCharacter->Debug) UE_LOG(LogTemp, Warning, TEXT("LockOnTarget Pressed"));

	if (!bIsLockedOn) {
		TArray<AActor*> actors = TraceForTarget();
		if (actors.Num() > 0)
		{
			AActor* newTarget = nullptr;
			if (targetActor == nullptr) {
				newTarget = GetTargetActor(actors);
			}

			if (newTarget) {
				ChangeTargetActor(newTarget);
			}
		}
	}
	else
	{
		ChangeTargetActor(nullptr);
	}
}

TArray<AActor*> UQuartzTargetLockComponent::TraceForTarget()
{
	// Get all actors in a zone around the player using multi-sphere trace
	TArray<FHitResult> hitResults;
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(OwningCharacter);
	TArray<AActor*> actorsToReturn;

	FVector pos = OwningCharacter->GetActorLocation();

	FCollisionShape shape = FCollisionShape::MakeSphere(lockOnDistance);

	bool hit = GetWorld()->SweepMultiByObjectType(
		hitResults,
		pos,
		pos,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		shape
	);


	if (hit)
	{
		for (const FHitResult& hitResult : hitResults)
		{
			AActor* actor = hitResult.GetActor();
			if (actor && !actorsToIgnore.Contains(actor) && !actorsToReturn.Contains(actor))
			{
				// check if actor is child of lockOnClass
				if (actor->IsA(lockOnClass)) {
					AQuartzEnemy* enemy = Cast<AQuartzEnemy>(actor);
					if (enemy && !enemy->IsDead()) {
						actorsToReturn.Add(actor);
					}
				}
			}
		}
	}

	// Debug draw the sphere
	if (OwningCharacter->Debug) DrawDebugSphere(GetWorld(), pos, lockOnDistance, 12, actorsToReturn.Num() > 0 ? FColor::Green : FColor::Red, false, 5.f);

	return actorsToReturn;

}

AActor* UQuartzTargetLockComponent::GetTargetActor(TArray<AActor*> actors)
{
	AActor* target = nullptr;

	double minDot = -1.0;

	for (AActor* actor : actors)
	{
		if (!actor) continue;
		// line trace from the camera towards the actor for a distance of lockOnDistance
		FHitResult hitResult;
		FVector start = OwningCharacter->GetFollowCamera()->GetComponentLocation();
		FVector end = actor->GetActorLocation();

		FCollisionQueryParams params;
		params.AddIgnoredActor(OwningCharacter);

		bool hit = GetWorld()->LineTraceSingleByObjectType(
			hitResult,
			start,
			end,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
			params
		);

		// Check the closest actor to the center of the screen
		FRotator r = UKismetMathLibrary::FindLookAtRotation(OwningCharacter->GetActorLocation(), actor->GetActorLocation());

		double dot = FVector::DotProduct(OwningCharacter->GetFollowCamera()->GetForwardVector(), r.Vector());

		if (dot > minDot)
		{
			minDot = dot;
			target = actor;
		}
	}
	// Debug draw the line
	if (OwningCharacter->Debug) DrawDebugLine(GetWorld(), OwningCharacter->GetFollowCamera()->GetComponentLocation(), target->GetActorLocation(), FColor::Red, false, 5.f);

	return target;
}

// TODO: merge with GetTargetActor
AActor* UQuartzTargetLockComponent::GetTargetActorSwitch(TArray<AActor*> actors, EDirection direction)
{
	AActor* target = nullptr;
	double minDot = -1.0;

	for (AActor* actor : actors)
	{
		if (!actor) continue;

		FHitResult hitResult;
		FVector start = OwningCharacter->GetFollowCamera()->GetComponentLocation();
		FVector end = actor->GetActorLocation();

		FCollisionQueryParams params;
		params.AddIgnoredActor(OwningCharacter);
		if (targetActor != nullptr) params.AddIgnoredActor(targetActor);

		bool hit = GetWorld()->LineTraceSingleByObjectType(
			hitResult,
			start,
			end,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
			params
		);

		double dot = FVector::DotProduct(OwningCharacter->GetFollowCamera()->GetRightVector(), hitResult.Normal);
		bool isCorrectSide = (direction == EDirection::Left) ? (asin(dot) > 0) : (asin(dot) < 0);

		if (isCorrectSide)
		{
			FRotator r = UKismetMathLibrary::FindLookAtRotation(OwningCharacter->GetActorLocation(), actor->GetActorLocation());
			dot = FVector::DotProduct(OwningCharacter->GetFollowCamera()->GetForwardVector(), r.Vector());

			if (dot > minDot)
			{
				minDot = dot;
				target = actor;
			}
		}
	}

	return target;
}

void UQuartzTargetLockComponent::SwitchTargetLock(const FInputActionValue& Value)
{
	if (bIsLockedOn) {
		FVector2D InputAxisVector = Value.Get<FVector2D>();

		AActor* newTarget = nullptr;
		EDirection direction = InputAxisVector.X > 0 ? EDirection::Right : EDirection::Left;
		newTarget = GetTargetActorSwitch(TraceForTarget(), direction);

		if (OwningCharacter->Debug) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("New Target: %s"), newTarget ? *newTarget->GetName() : TEXT("None")));

		if (newTarget != nullptr) {
			ChangeTargetActor(newTarget);
		}
	}
}

// TODO: refactor
void UQuartzTargetLockComponent::UpdateTargetLock()
{
	if (!bIsLockedOn || targetActor == nullptr)
	{
		bIsLockedOn = false;
		ChangeTargetActor(nullptr);
		return;
	}
	else
	{
		AQuartzEnemy* enemy = Cast<AQuartzEnemy>(targetActor);
		if (!IsValid(targetActor) || enemy->IsDead()) {
			AActor* newTarget = nullptr;
			newTarget = GetTargetActorSwitch(TraceForTarget(), EDirection::Right);

			if (OwningCharacter->Debug) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("New Target: %s"), newTarget ? *newTarget->GetName() : TEXT("None")));

			if (newTarget != nullptr) {
				ChangeTargetActor(newTarget);
			}
			else
			{
				bIsLockedOn = false;
				ChangeTargetActor(nullptr);
			}
			return;
		}
		FVector start = OwningCharacter->GetActorLocation();
		FVector end = targetActor->GetActorLocation();

		float distance = FVector::Dist(start, end);

		if (distance > enemy->GetLockOutDistance()) {
			bIsLockedOn = false;
		}
		else {

			FRotator currentRotation = OwningCharacter->GetController()->GetControlRotation();

			FRotator targetRotation = GetLockOnRotation();

			float deltaTime = GetWorld()->GetDeltaSeconds();
			float interpSpeed = interpolationSpeed;

			FRotator newRotation = FMath::RInterpTo(currentRotation, targetRotation, deltaTime, interpSpeed);

			// clamp the pitch to prevent the player from looking up or down too much
			newRotation.Pitch = FMath::ClampAngle(newRotation.Pitch, -clampAngle, clampAngle);

			// Set the rotation of the camera
			OwningCharacter->GetController()->SetControlRotation(newRotation);

			APlayerController* PlayerController = OwningCharacter->GetController<APlayerController>();

		}
	}
}

FVector UQuartzTargetLockComponent::GetTargetLocation()
{
	if (targetActor != nullptr)
	{
		return targetActor->GetActorLocation();
	}
	else
	{
		return FVector(0, 0, 0);
	}
}

void UQuartzTargetLockComponent::SetLockTimer(bool IsLocked)
{
	if (IsLocked) {
		GetWorld()->GetTimerManager().SetTimer(TargetLockTimerHandle, this, &UQuartzTargetLockComponent::UpdateTargetLock, GetWorld()->GetDeltaSeconds(), true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TargetLockTimerHandle);
	}
}

FRotator UQuartzTargetLockComponent::GetLockOnRotation()
{
	FVector start = OwningCharacter->GetFollowCamera()->GetComponentLocation();

	FVector targetPos = targetActor->GetActorLocation();

	double dist = FVector::Dist(start, targetPos);

	FVector end = FVector(targetPos.X, targetPos.Y, targetPos.Z - (dist / distanceFactor));

	FVector Direction = end - start;

	FRotator Rot = FRotationMatrix::MakeFromX(Direction).Rotator();


	return Rot;
}

void UQuartzTargetLockComponent::ChangeTargetActor(AActor* newTarget)
{
	targetActor = newTarget;
	if (newTarget != nullptr)
	{
		bIsLockedOn = true;
		SetLockTimer(true);
		OnLockTarget();
	}
	else
	{
		SetLockTimer(false);
		bIsLockedOn = false;
		OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwningCharacter->bUseControllerRotationYaw = false; 
		OnUnlockTarget();
	}
}




// Called every frame
void UQuartzTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

