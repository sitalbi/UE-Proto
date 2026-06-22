// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuartzTargetLockComponent.h"
#include "GameFramework/Character.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/QuartzTargetLockableInterface.h"

UQuartzTargetLockComponent::UQuartzTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	OwningCharacter = Cast<AQuartzPlayerCharacter>(GetOwner());
}

void UQuartzTargetLockComponent::LockOnTarget(const FInputActionValue& Value)
{
	if(OwningCharacter->Debug) UE_LOG(LogTemp, Warning, TEXT("LockOnTarget Pressed"));

	if (!bIsLockedOn)
	{
		TArray<AActor*> Actors = TraceForTarget();

		if (Actors.Num() > 0)
		{
			AActor* NewTarget = GetTargetActor(Actors);

			if (NewTarget)
			{
				ChangeTargetActor(NewTarget);
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

	FCollisionShape shape = FCollisionShape::MakeSphere(MaxLockSearchDistance);

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

			if (!actor || actorsToIgnore.Contains(actor) || actorsToReturn.Contains(actor))
			{
				continue;
			}

			if (IsLockableTarget(actor))
			{
				const float DistanceToActor = FVector::Dist(OwningCharacter->GetActorLocation(), GetLockableLocation(actor));

				const float ActorLockOnDistance = GetLockableLockOnDistance(actor);

				if (DistanceToActor <= ActorLockOnDistance)
				{
					actorsToReturn.Add(actor);
				}
			}
		}
	}

	// Debug draw the sphere
	if (OwningCharacter->Debug) DrawDebugSphere(GetWorld(), pos, MaxLockSearchDistance, 12, actorsToReturn.Num() > 0 ? FColor::Green : FColor::Red, false, 5.f);

	return actorsToReturn;

}

AActor* UQuartzTargetLockComponent::GetTargetActor(const TArray<AActor*>& Actors)
{
	AActor* BestTarget = nullptr;
	float BestDot = -1.0f;

	if (!OwningCharacter || !OwningCharacter->GetFollowCamera())
	{
		return nullptr;
	}

	const FVector CameraLocation = OwningCharacter->GetFollowCamera()->GetComponentLocation();
	const FVector CameraForward = OwningCharacter->GetFollowCamera()->GetForwardVector();

	for (AActor* Actor : Actors)
	{
		if (!IsLockableTarget(Actor))
		{
			continue;
		}

		const FVector TargetLocation = GetLockableLocation(Actor);
		const FVector ToTarget = (TargetLocation - CameraLocation).GetSafeNormal();

		const float Dot = FVector::DotProduct(CameraForward, ToTarget);

		if (Dot > BestDot)
		{
			BestDot = Dot;
			BestTarget = Actor;
		}
	}

	if (OwningCharacter->Debug && IsValid(BestTarget))
	{
		DrawDebugLine(GetWorld(), CameraLocation, GetLockableLocation(BestTarget), FColor::Red, false, 5.f);
	}

	return BestTarget;
}

AActor* UQuartzTargetLockComponent::GetTargetActorSwitch(const TArray<AActor*>& actors, EDirection direction)
{
	AActor* Target = nullptr;
	float BestForwardDot = -1.0f;

	if (!OwningCharacter || !OwningCharacter->GetFollowCamera())
	{
		return nullptr;
	}

	const FVector CameraLocation = OwningCharacter->GetFollowCamera()->GetComponentLocation();
	const FVector CameraForward = OwningCharacter->GetFollowCamera()->GetForwardVector();
	const FVector CameraRight = OwningCharacter->GetFollowCamera()->GetRightVector();

	for (AActor* Actor : actors)
	{
		if (!IsLockableTarget(Actor) || Actor == TargetActor)
		{
			continue;
		}

		const FVector ActorLocation = GetLockableLocation(Actor);
		const FVector ToActor = (ActorLocation - CameraLocation).GetSafeNormal();

		const float SideDot = FVector::DotProduct(CameraRight, ToActor);

		const bool bIsCorrectSide = direction == EDirection::Right
			? SideDot > 0.0f
			: SideDot < 0.0f;

		if (!bIsCorrectSide)
		{
			continue;
		}

		const float ForwardDot = FVector::DotProduct(CameraForward, ToActor);

		if (ForwardDot > BestForwardDot)
		{
			BestForwardDot = ForwardDot;
			Target = Actor;
		}
	}

	return Target;
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

void UQuartzTargetLockComponent::UpdateTargetLock()
{
	if (!bIsLockedOn || IsValid(TargetActor))
	{
		ChangeTargetActor(nullptr);
		return;
	}
	else
	{
		if (!IsLockableTarget(TargetActor))
		{
			AActor* newTarget = GetTargetActor(TraceForTarget());

			if (OwningCharacter->Debug)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.f,
					FColor::Red,
					FString::Printf(TEXT("New Target: %s"), newTarget ? *newTarget->GetName() : TEXT("None"))
				);
			}

			ChangeTargetActor(newTarget);
			return;
		}
		const FVector Start = OwningCharacter->GetActorLocation();
		const FVector End = GetLockableLocation(TargetActor);

		const float Distance = FVector::Dist(Start, End);

		const float LockOutDistance = GetLockableLockOutDistance(TargetActor);

		if (Distance > LockOutDistance)
		{
			ChangeTargetActor(nullptr);
			return;
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
		}
	}
}

FVector UQuartzTargetLockComponent::GetTargetLocation()
{
	if (IsLockableTarget(TargetActor))
	{
		return GetLockableLocation(TargetActor);
	}

	return FVector::ZeroVector;
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

	FVector targetPos = GetTargetLocation();

	double dist = FVector::Dist(start, targetPos);

	FVector end = FVector(targetPos.X, targetPos.Y, targetPos.Z - (dist / distanceFactor));

	FVector Direction = end - start;

	FRotator Rot = FRotationMatrix::MakeFromX(Direction).Rotator();


	return Rot;
}

void UQuartzTargetLockComponent::ChangeTargetActor(AActor* newTarget)
{
	TargetActor = newTarget;
	if (newTarget != nullptr)
	{
		bIsLockedOn = true;
		SetLockTimer(true);
		OwningCharacter->bUseControllerRotationYaw = true;
		OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
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





bool UQuartzTargetLockComponent::IsLockableTarget(AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return false;
	}

	if (!Actor->GetClass()->ImplementsInterface(UQuartzTargetLockableInterface::StaticClass()))
	{
		return false;
	}

	return IQuartzTargetLockableInterface::Execute_CanBeLockedOn(Actor);
}

FVector UQuartzTargetLockComponent::GetLockableLocation(AActor* Actor) const
{
	if (!IsLockableTarget(Actor))
	{
		return FVector::ZeroVector;
	}

	return IQuartzTargetLockableInterface::Execute_GetLockOnLocation(Actor);
}

float UQuartzTargetLockComponent::GetLockableLockOnDistance(AActor* Actor) const
{
	if (!IsLockableTarget(Actor))
	{
		return 0.0f;
	}

	return IQuartzTargetLockableInterface::Execute_GetLockOnDistance(Actor);
}

float UQuartzTargetLockComponent::GetLockableLockOutDistance(AActor* Actor) const
{
	if (!IsLockableTarget(Actor))
	{
		return 0.0f;
	}

	return IQuartzTargetLockableInterface::Execute_GetLockOutDistance(Actor);
}

// Called every frame
void UQuartzTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}