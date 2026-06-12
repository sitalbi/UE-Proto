#include "Characters/QuartzEnemy.h"


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

bool AQuartzEnemy::IsDead() const
{
	return false;
}

