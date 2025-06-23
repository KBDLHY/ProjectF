// Fill out your copyright notice in the Description page of Project Settings.


#include "InputData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputData)

const UInputAction* UInputData::FindNativeInputActionForInputTag(const FGameplayTag& InputTag) const
{
	for (const FInputConfig& InputConfig : NativeInputConfigs)
	{
		if (InputConfig.InputTag.MatchesTag(InputTag))
		{
			return InputConfig.InputAction;
		}
	}
	return nullptr;
}
