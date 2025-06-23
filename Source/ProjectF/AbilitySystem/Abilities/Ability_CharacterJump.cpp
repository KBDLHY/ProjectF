// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_CharacterJump.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/Character.h"
#include "ProjectF/BaseGameplayTags.h"

UAbility_CharacterJump::UAbility_CharacterJump()
{
	SetAssetTags(FGameplayTagContainer(BaseGameplayTags::Ability_Jump));
	
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
}

bool UAbility_CharacterJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!IsValid(Character) || !Character->CanJump())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAbility_CharacterJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	StartJump();

	if (UAbilityTask_WaitInputRelease* Task = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		Task->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		Task->ReadyForActivation();
	}
}

void UAbility_CharacterJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopJump();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbility_CharacterJump::StartJump() const
{
	if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (Character->IsLocallyControlled() && !Character->bPressedJump)
		{
			Character->UnCrouch();
			Character->Jump();
		}
	}
}

void UAbility_CharacterJump::StopJump() const
{
	if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (Character->IsLocallyControlled() && Character->bPressedJump)
		{
			Character->StopJumping();
		}
	}
}

void UAbility_CharacterJump::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
