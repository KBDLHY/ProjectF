// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

PROJECTF_API DECLARE_LOG_CATEGORY_EXTERN(LogPF, Log, All);
PROJECTF_API DECLARE_LOG_CATEGORY_EXTERN(LogPFExperience, Log, All);
PROJECTF_API DECLARE_LOG_CATEGORY_EXTERN(LogPFAbilitySystem, Log, All);
PROJECTF_API DECLARE_LOG_CATEGORY_EXTERN(LogPFTeams, Log, All);

PROJECTF_API FString GetClientServerContextString(UObject* ContextObject = nullptr);