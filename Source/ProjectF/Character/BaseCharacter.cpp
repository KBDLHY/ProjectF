// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "ProjectF/BaseGameplayTags.h"
#include "ProjectF/AbilitySystem/BaseAbilitySystemComponent.h"
#include "ProjectF/AbilitySystem/Abilities/Ability_CharacterJump.h"
#include "ProjectF/Player/BasePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseCharacter)

ABaseCharacter::ABaseCharacter()
{
	
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystem();
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAbilitySystemComponent* ABaseCharacter::GetBaseAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAttributeSet* ABaseCharacter::GetBaseAttributeSet() const
{
	return AttributeSet;
}

void ABaseCharacter::InitAbilitySystem()
{
	if (ABasePlayerState* BasePlayerState = GetPlayerState<ABasePlayerState>())
	{
		AbilitySystemComponent = BasePlayerState->GetBaseAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(BasePlayerState, this);
		AttributeSet = BasePlayerState->GetBaseAttributeSet();
	}

	// TODO : For Test
	if (HasAuthority() && AbilitySystemComponent)
	{
		FGameplayAbilitySpec Spec(UAbility_CharacterJump::StaticClass());
		Spec.GetDynamicSpecSourceTags().AddTag(BaseGameplayTags::Input_Jump);
		AbilitySystemComponent->GiveAbility(Spec);
	}
}

