#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "InputAction.h"
#include "QuartzAbilityInputTypes.generated.h"

UENUM(BlueprintType)
enum class EQuartzAbilityInputID : uint8
{
    None = 0,
    Dash
};


USTRUCT(BlueprintType)
struct QUARTZ_API FQuartzAbilityBind
{
    GENERATED_BODY()

    // The ability to grant
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UGameplayAbility> Ability;

    // Input Action from Enhanced Input
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UInputAction* InputAction = nullptr;

    // Input ID used by GAS
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EQuartzAbilityInputID InputID = EQuartzAbilityInputID::None;
};
