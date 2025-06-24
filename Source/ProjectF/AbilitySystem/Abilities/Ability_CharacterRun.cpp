// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_CharacterRun.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectF/BaseGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Ability_CharacterRun)

UAbility_CharacterRun::UAbility_CharacterRun()
{
	SetAssetTags(FGameplayTagContainer(BaseGameplayTags::Ability_Run));
	
	ActivationPolicy = EAbilityActivationPolicy::WhileInputActive;
}

bool UAbility_CharacterRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

void UAbility_CharacterRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	StartRun();

	if (UAbilityTask_WaitInputRelease* Task = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		Task->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		Task->ReadyForActivation();
	}
}

void UAbility_CharacterRun::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	StopRun();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbility_CharacterRun::StartRun() const
{
	if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (auto* Movement = Character->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = 600.f;
		}
	}
}

void UAbility_CharacterRun::StopRun() const
{
	if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo ? CurrentActorInfo->AvatarActor : nullptr))
	{
		if (auto* Movement = Character->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = 300.f;
		}
	}
}

void UAbility_CharacterRun::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
