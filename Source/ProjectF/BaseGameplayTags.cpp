
#include "BaseGameplayTags.h"

namespace BaseGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Jump, TEXT("Input.Jump"));
	UE_DEFINE_GAMEPLAY_TAG(Input_Move, TEXT("Input.Move"));
	UE_DEFINE_GAMEPLAY_TAG(Input_Look, TEXT("Input.Look"));
	UE_DEFINE_GAMEPLAY_TAG(Input_Crouch, TEXT("Input.Crouch"));
	UE_DEFINE_GAMEPLAY_TAG(Input_Run, TEXT("Input.Run"));

	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, TEXT("Ability.Jump"));
	UE_DEFINE_GAMEPLAY_TAG(Ability_Move, TEXT("Ability.Move"));
	UE_DEFINE_GAMEPLAY_TAG(Ability_Look, TEXT("Ability.Look"));
	UE_DEFINE_GAMEPLAY_TAG(Ability_Crouch, TEXT("Ability.Crouch"));
	UE_DEFINE_GAMEPLAY_TAG(Ability_Run, TEXT("Ability.Run"));

	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Interact_WeaponPickup, TEXT("GameplayCue.Interact.WeaponPickup"));

	UE_DEFINE_GAMEPLAY_TAG(Event_Movement_Reload, TEXT("Event.Movement.Reload"));

	UE_DEFINE_GAMEPLAY_TAG(Weapon_MagazineSize, TEXT("Weapon.MagazineSize"));
	UE_DEFINE_GAMEPLAY_TAG(Weapon_MagazineAmmo, TEXT("Weapon.MagazineAmmo"));
	UE_DEFINE_GAMEPLAY_TAG(Weapon_SpareAmmo, TEXT("Weapon.SpareAmmo"));
	
	UE_DEFINE_GAMEPLAY_TAG(Cosmetic_AnimationStyle, TEXT("Cosmetic.AnimationStyle"));
}
