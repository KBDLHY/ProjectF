// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

struct FInputActionValue;
class UBaseAbilitySystemComponent;
class UInputData;

UCLASS()
class PROJECTF_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void Input_Move(const FInputActionValue& InputValue);
	void Input_Look(const FInputActionValue& InputValue);

private:
	UPROPERTY()
	TObjectPtr<UInputData> InputData;

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	UBaseAbilitySystemComponent* GetBaseAbilitySystemComponent() const;
};
