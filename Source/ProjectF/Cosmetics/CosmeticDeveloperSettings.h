// Copyright Epic Games, Inc. All Rights Reserved.
#if 0
#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"

#include "CosmeticDeveloperSettings.generated.h"

struct FCharacterPart;
struct FPropertyChangedEvent;

class UExperienceDefinition;

UENUM()
enum class ECosmeticCheatMode
{
	ReplaceParts,

	AddParts
};

/**
 * Cosmetic developer settings / editor cheats
 */
UCLASS(config=EditorPerProjectUserSettings, MinimalAPI)
class UCosmeticDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UCosmeticDeveloperSettings();

	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface

public:
	UPROPERTY(Transient, EditAnywhere)
	TArray<FCharacterPart> CheatCosmeticCharacterParts;

	UPROPERTY(Transient, EditAnywhere)
	ECosmeticCheatMode CheatMode;

#if WITH_EDITOR
public:
	// Called by the editor engine to let us pop reminder notifications when cheats are active
	PROJECTF_API void OnPlayInEditorStarted() const;

private:
	void ApplySettings();
	void ReapplyLoadoutIfInPIE();
#endif

public:
	//~UObject interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostReloadConfig(FProperty* PropertyThatWasLoaded) override;
	virtual void PostInitProperties() override;
#endif
	//~End of UObject interface

private:


};
#endif