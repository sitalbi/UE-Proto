// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Abilities/GameplayAbility.h>
#include "QuartzWeaponData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class QUARTZ_API UQuartzWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag WeaponTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* LightAttackMontage;
};
