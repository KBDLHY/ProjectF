// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerState.h"

#include "ProjectF/AbilitySystem/BaseAbilitySystemComponent.h"
#include "ProjectF/AbilitySystem/BaseAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BasePlayerState)

ABasePlayerState::ABasePlayerState()
{
	SetNetUpdateFrequency(120.f);
	SetMinNetUpdateFrequency(60.f);

	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ABasePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAbilitySystemComponent* ABasePlayerState::GetBaseAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAttributeSet* ABasePlayerState::GetBaseAttributeSet() const
{
	return AttributeSet;
}
