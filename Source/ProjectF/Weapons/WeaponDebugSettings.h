// Copyright Epic Games, Inc. All Rights Reserved.
#if 0
#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"

#include "WeaponDebugSettings.generated.h"

class UObject;

/**
 * Developer debugging settings for weapons
 */
UCLASS(config=EditorPerProjectUserSettings)
class UWeaponDebugSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UWeaponDebugSettings();

	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface

public:
	// Should we do debug drawing for bullet traces (if above zero, sets how long (in seconds)
	UPROPERTY(config, EditAnywhere, Category=General, meta=(ConsoleVariable=".Weapon.DrawBulletTraceDuration", ForceUnits=s))
	float DrawBulletTraceDuration;

	// Should we do debug drawing for bullet impacts (if above zero, sets how long (in seconds)
	UPROPERTY(config, EditAnywhere, Category = General, meta = (ConsoleVariable = ".Weapon.DrawBulletHitDuration", ForceUnits = s))
	float DrawBulletHitDuration;

	// When bullet hit debug drawing is enabled (see DrawBulletHitDuration), how big should the hit radius be? (in cm)
	UPROPERTY(config, EditAnywhere, Category = General, meta = (ConsoleVariable = ".Weapon.DrawBulletHitRadius", ForceUnits=cm))
	float DrawBulletHitRadius;
};
#endif