#pragma once
#include "NativeGameplayTags.h"

namespace QuartzTags
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Weapon_Equipped);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dash);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Dash);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dashing);

    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Light);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Attack_Light);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Attacking);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stunned);


    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Input_LightAttack);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combo_Window_Open);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combo_Window_Close);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combo_Commit);
}
