#include "QuartzEnemy.h"


AQuartzEnemy::AQuartzEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

}


void AQuartzEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}


void AQuartzEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AQuartzEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AQuartzEnemy::IsDead() const
{
	return false;
}

