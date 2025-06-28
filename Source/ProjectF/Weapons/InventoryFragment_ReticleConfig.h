// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/InventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UReticleWidgetBase;
class UObject;

UCLASS()
class UInventoryFragment_ReticleConfig : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<UReticleWidgetBase>> ReticleWidgets;
};
