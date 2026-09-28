#include "Components/QuartzTargetLockComponent.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Interfaces/QuartzTargetLockableInterface.h"


UQuartzTargetLockComponent::UQuartzTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


void UQuartzTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<AQuartzPlayerCharacter>(GetOwner());

	if (!IsValid(OwningCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("QuartzTargetLockComponent owner is not AQuartzPlayerCharacter"));
		return;
	}

	SetComponentTickEnabled(false);
}


void UQuartzTargetLockComponent::BindInput(UEnhancedInputComponent* PlayerInputComponent)
{
	if (!PlayerInputComponent)
	{
		return;
	}

	if (TargetLockAction)
	{
		PlayerInputComponent->BindAction(TargetLockAction, ETriggerEvent::Started, this, &UQuartzTargetLockComponent::LockOnTarget);
	}

	if (SwitchTargetLockAction)
	{
		PlayerInputComponent->BindAction(SwitchTargetLockAction, ETriggerEvent::Triggered, this, &UQuartzTargetLockComponent::SwitchTargetLock);
	}
}


void UQuartzTargetLockComponent::LockOnTarget(const FInputActionValue& Value)
{
	if (!IsValid(OwningCharacter))
	{
		return;
	}

	if (OwningCharacter->Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("LockOnTarget Pressed"));
	}

	if (bIsLockedOn)
	{
		ChangeTargetActor(nullptr);
		return;
	}

	const TArray<AActor*> Actors = TraceForTarget();

	if (Actors.IsEmpty())
	{
		return;
	}

	AActor* NewTarget = GetTargetActor(Actors);

	if (IsValid(NewTarget))
	{
		ChangeTargetActor(NewTarget);
	}
}


void UQuartzTargetLockComponent::SwitchTargetLock(const FInputActionValue& Value)
{
	if (!bIsLockedOn || !IsValid(OwningCharacter))
	{
		return;
	}

	const FVector2D InputAxisVector = Value.Get<FVector2D>();

	if (FMath::IsNearlyZero(InputAxisVector.X))
	{
		return;
	}

	const EDirection Direction = InputAxisVector.X > 0.0f ? EDirection::Right : EDirection::Left;

	AActor* NewTarget = GetTargetActorSwitch(TraceForTarget(), Direction);

	if (OwningCharacter->Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("New Target: %s"), *GetNameSafe(NewTarget));
	}

	if (IsValid(NewTarget))
	{
		ChangeTargetActor(NewTarget);
	}
}


void UQuartzTargetLockComponent::ChangeTargetActor(AActor* NewTarget)
{
	if (!IsValid(OwningCharacter))
	{
		return;
	}

	TargetActor = NewTarget;

	if (IsValid(TargetActor))
	{
		bIsLockedOn = true;

		OwningCharacter->bUseControllerRotationYaw = true;
		OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;

		SetComponentTickEnabled(true);

		OnLockTarget();

		UpdateTargetLock(0.0f);
	}
	else
	{
		TargetActor = nullptr;
		bIsLockedOn = false;

		SetComponentTickEnabled(false);

		OwningCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		OwningCharacter->bUseControllerRotationYaw = false;

		OnUnlockTarget();
	}
}


void UQuartzTargetLockComponent::UpdateTargetLock(float DeltaTime)
{
	if (!IsValid(OwningCharacter) || !bIsLockedOn || !IsValid(TargetActor))
	{
		ChangeTargetActor(nullptr);
		return;
	}

	if (!IsLockableTarget(TargetActor))
	{
		ChangeTargetActor(nullptr);
		return;
	}

	const FVector TargetLocation = GetLockableLocation(TargetActor);
	const float Distance = FVector::Dist(OwningCharacter->GetActorLocation(), TargetLocation);
	const float LockOutDistance = GetLockableLockOutDistance(TargetActor);

	if (Distance > LockOutDistance)
	{
		ChangeTargetActor(nullptr);
		return;
	}

	OnUpdateTargetLock();

	if (AController* Controller = OwningCharacter->GetController())
	{
		const FRotator CurrentRotation = Controller->GetControlRotation();
		const FRotator TargetRotation = GetLockOnRotation();

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, interpolationSpeed);

		NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -clampAngle, clampAngle);

		Controller->SetControlRotation(NewRotation);
	}
}


void UQuartzTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTargetLock(DeltaTime);
}


TArray<AActor*> UQuartzTargetLockComponent::TraceForTarget()
{
	TArray<AActor*> ActorsToReturn;

	if (!IsValid(OwningCharacter) || !GetWorld())
	{
		return ActorsToReturn;
	}

	TArray<FHitResult> HitResults;

	const FVector Position = OwningCharacter->GetActorLocation();
	const FCollisionShape Shape = FCollisionShape::MakeSphere(MaxLockSearchDistance);

	const bool bHit = GetWorld()->SweepMultiByObjectType(HitResults, Position, Position, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), Shape);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* Actor = HitResult.GetActor();

			if (!IsValid(Actor) || Actor == OwningCharacter || ActorsToReturn.Contains(Actor))
			{
				continue;
			}

			if (!IsLockableTarget(Actor))
			{
				continue;
			}

			const float DistanceToActor = FVector::Dist(OwningCharacter->GetActorLocation(), GetLockableLocation(Actor));
			const float LockOnDistance = GetLockableLockOnDistance(Actor);

			if (DistanceToActor <= LockOnDistance)
			{
				ActorsToReturn.Add(Actor);
			}
		}
	}

	if (OwningCharacter->Debug)
	{
		DrawDebugSphere(GetWorld(), Position, MaxLockSearchDistance, 12, ActorsToReturn.IsEmpty() ? FColor::Red : FColor::Green, false, 5.0f);
	}

	return ActorsToReturn;
}


AActor* UQuartzTargetLockComponent::GetTargetActor(const TArray<AActor*>& Actors)
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetFollowCamera())
	{
		return nullptr;
	}

	AActor* BestTarget = nullptr;
	float BestDot = -1.0f;

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
		DrawDebugLine(GetWorld(), CameraLocation, GetLockableLocation(BestTarget), FColor::Red, false, 5.0f);
	}

	return BestTarget;
}


AActor* UQuartzTargetLockComponent::GetTargetActorSwitch(const TArray<AActor*>& Actors, EDirection Direction)
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetFollowCamera())
	{
		return nullptr;
	}

	AActor* BestTarget = nullptr;
	float BestForwardDot = -1.0f;

	const FVector CameraLocation = OwningCharacter->GetFollowCamera()->GetComponentLocation();
	const FVector CameraForward = OwningCharacter->GetFollowCamera()->GetForwardVector();
	const FVector CameraRight = OwningCharacter->GetFollowCamera()->GetRightVector();

	for (AActor* Actor : Actors)
	{
		if (!IsLockableTarget(Actor) || Actor == TargetActor)
		{
			continue;
		}

		const FVector ActorLocation = GetLockableLocation(Actor);
		const FVector ToActor = (ActorLocation - CameraLocation).GetSafeNormal();

		const float SideDot = FVector::DotProduct(CameraRight, ToActor);
		const bool bCorrectSide = Direction == EDirection::Right ? SideDot > 0.0f : SideDot < 0.0f;

		if (!bCorrectSide)
		{
			continue;
		}

		const float ForwardDot = FVector::DotProduct(CameraForward, ToActor);

		if (ForwardDot > BestForwardDot)
		{
			BestForwardDot = ForwardDot;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}


FVector UQuartzTargetLockComponent::GetTargetLocation() const
{
	if (IsLockableTarget(TargetActor))
	{
		return GetLockableLocation(TargetActor);
	}

	return FVector::ZeroVector;
}


FRotator UQuartzTargetLockComponent::GetLockOnRotation() const
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetFollowCamera() || !IsLockableTarget(TargetActor))
	{
		return FRotator::ZeroRotator;
	}

	const FVector CameraLocation = OwningCharacter->GetFollowCamera()->GetComponentLocation();
	const FVector TargetLocation = GetLockableLocation(TargetActor);
	const double Distance = FVector::Dist(CameraLocation, TargetLocation);

	const FVector AdjustedTargetLocation(TargetLocation.X, TargetLocation.Y, TargetLocation.Z - Distance / distanceFactor);

	return (AdjustedTargetLocation - CameraLocation).Rotation();
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