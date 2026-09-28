#include "Characters/QuartzEnemy.h"
#include "QuartzGameplayTags.h"


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
	return HealthSet->GetHealth() <= 0;
}

bool AQuartzEnemy::Attack()
{
	if (!AbilitySystemComponent)
	{
		return false;
	}

	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(QuartzTags::Ability_Attack_Light);

	return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags);
}
