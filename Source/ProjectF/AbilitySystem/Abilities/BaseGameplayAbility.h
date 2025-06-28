// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 어빌리티를 활성화하는 방법을 정의하는 enum class
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	// 코드를 통해 수동으로 어빌리티 활성화
	Manual,

	// 입력이 들어오면 어빌리티 활성화 시도
	OnInputTriggered,

	// 입력이 켜진 동안 계속해서 어빌리티 활성화 시도
	WhileInputActive,

	// Avatar Actor가 ASC에 등록될 때 어빌리티 활성화 시도
	OnGiveOrSpawn
};

/**
 * 프로젝트의 모든 Gameplay Ability가 상속받아야 하는 최상위 추상 클래스
 */
UCLASS(Abstract)
class PROJECTF_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	
	UFUNCTION(BlueprintCallable, Category = "ProjectF|Ability")
	AController* GetControllerFromActorInfo() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	EAbilityActivationPolicy ActivationPolicy;
};
