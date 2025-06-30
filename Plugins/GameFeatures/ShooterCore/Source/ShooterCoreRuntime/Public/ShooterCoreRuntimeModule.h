// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"

class FShooterCoreRuntimeModule : public IModuleInterface
{
public:
	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
