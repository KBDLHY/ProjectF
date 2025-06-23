// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseGameplayAbility)

UBaseGameplayAbility::UBaseGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	
	ActivationPolicy = EAbilityActivationPolicy::Manual;
}
