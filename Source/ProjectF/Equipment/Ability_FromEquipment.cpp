// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ability_FromEquipment.h"
#include "EquipmentInstance.h"
#include "Inventory/InventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(Ability_FromEquipment)

UAbility_FromEquipment::UAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UEquipmentInstance* UAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UInventoryItemInstance* UAbility_FromEquipment::GetAssociatedItem() const
{
	if (UEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
				{
					Context.AddError(NSLOCTEXT("", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
					Result = EDataValidationResult::Invalid;
				}

	return Result;
}

#endif
