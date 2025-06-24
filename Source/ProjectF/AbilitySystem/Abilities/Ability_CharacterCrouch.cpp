// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_CharacterCrouch.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ProjectF/BaseGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Ability_CharacterCrouch)

UAbility_CharacterCrouch::UAbility_CharacterCrouch()
{
	SetAssetTags(FGameplayTagContainer(BaseGameplayTags::Ability_Crouch));
	
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
}

bool UAbility_CharacterCrouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}
	
	const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!IsValid(Character) || nullptr == Character->GetMovementComponent())
	{
		return false;
	}
	
	if (!Character->bIsCrouched && !Character->CanCrouch())
	{
		return false;
	}
	
	if (const auto* Movement = Character->GetMovementComponent())
	{
		if (!Movement->IsMovingOnGround() || Movement->IsFalling())
		{
			return false;
		}
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAbility_CharacterCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	ToggleCrouch();

	if (UAbilityTask_WaitInputRelease* Task = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		Task->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		Task->ReadyForActivation();
	}
}

void UAbility_CharacterCrouch::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbility_CharacterCrouch::ToggleCrouch() const
{
	if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
		}
		else
		{
			Character->Crouch();
		}
	}
}

void UAbility_CharacterCrouch::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}