// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbilitySystemComponent.h"

#include "Abilities/BaseGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseAbilitySystemComponent)

void UBaseAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive() && Spec.GetPrimaryInstance())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		// @see UAbilityTask_WaitInputPress
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey());
	}
}

void UBaseAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive() && Spec.GetPrimaryInstance())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		// @see UAbilityTask_WaitInputPress
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.GetPrimaryInstance()->GetCurrentActivationInfo().GetActivationPredictionKey());
	}
}

void UBaseAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				InputPressedSpecHandles.AddUnique(Spec.Handle);
				InputHeldSpecHandles.AddUnique(Spec.Handle);
			}
		}
	}
}

void UBaseAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				InputReleasedSpecHandles.AddUnique(Spec.Handle);
				InputHeldSpecHandles.Remove(Spec.Handle);
			}
		}
	}
}

void UBaseAbilitySystemComponent::ProcessAbilityInput()
{
	// TODO : Block Process Input?

	// 배열의 capacity를 유지해 메모리 할당을 줄임
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// 입력이 유지될 때 활성화되는 어빌리티 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (!Spec->IsActive())
			{
				const UBaseGameplayAbility* AbilityCDO = CastChecked<UBaseGameplayAbility>(Spec->Ability);
				if (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	// 이번 프레임에 input press된 어빌리티 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			Spec->InputPressed = true;
			
			if (Spec->IsActive())
			{
				AbilitySpecInputPressed(*Spec);
			}
			else
			{
				const UBaseGameplayAbility* AbilityCDO = CastChecked<UBaseGameplayAbility>(Spec->Ability);
				if (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
				{
					AbilitiesToActivate.AddUnique(SpecHandle);
				}
			}
		}
	}

	// 어빌리티 활성화
	for (const FGameplayAbilitySpecHandle& SpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(SpecHandle);
	}

	// 이번 프레임에 input release된 어빌리티 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
		{
			Spec->InputPressed = false;
			
			if (Spec->IsActive())
			{
				AbilitySpecInputReleased(*Spec);
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}
