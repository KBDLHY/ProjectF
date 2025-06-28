// Copyright Epic Games, Inc. All Rights Reserved.
#if 0
#include "WeaponDebugSettings.h"
#include "Misc/App.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponDebugSettings)

UWeaponDebugSettings::UWeaponDebugSettings()
{
}

FName UWeaponDebugSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#endif