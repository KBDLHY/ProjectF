// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "BasePlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectF/BaseGameplayTags.h"
#include "ProjectF/AbilitySystem/BaseAbilitySystemComponent.h"
#include "ProjectF/Data/InputData.h"
#include "ProjectF/Input/BaseInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BasePlayerController)

ABasePlayerController::ABasePlayerController()
{
	const ConstructorHelpers::FObjectFinder<UInputData> InputDataObject(TEXT("/Script/ProjectF.InputData'/Game/Data/DA_InputData.DA_InputData'"));
	if (InputDataObject.Succeeded())
	{
		InputData = InputDataObject.Object;
	}
}

void ABasePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	if (UBaseAbilitySystemComponent* ASC = GetBaseAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput();
	}
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(InputData);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputData->DefaultMappingContext, 0);
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputData);

	UBaseInputComponent* FPInputComponent = CastChecked<UBaseInputComponent>(InputComponent);

	// Bind Native Inputs
	FPInputComponent->BindNativeAction(InputData, BaseGameplayTags::Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	FPInputComponent->BindNativeAction(InputData, BaseGameplayTags::Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	
	// Bind Ability Inputs
	FPInputComponent->BindAbilityActions(InputData, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
}

void ABasePlayerController::Input_Move(const FInputActionValue& InputValue)
{
	if (GetPawn())
	{
		const FRotator Rotation = GetPawn()->GetControlRotation();
		const FVector RightDir = UKismetMathLibrary::GetRightVector(FRotator(0.f, Rotation.Yaw, 0.f));
		const FVector ForwardDir = UKismetMathLibrary::GetForwardVector(FRotator(0.f, Rotation.Yaw, 0.f));
		GetPawn()->AddMovementInput(RightDir, InputValue.Get<FVector2D>().X);
		GetPawn()->AddMovementInput(ForwardDir, InputValue.Get<FVector2D>().Y);
	}
}

void ABasePlayerController::Input_Look(const FInputActionValue& InputValue)
{
	if (GetPawn())
	{
		GetPawn()->AddControllerYawInput(InputValue.Get<FVector2D>().X);
		GetPawn()->AddControllerPitchInput(InputValue.Get<FVector2D>().Y);
	}
}

void ABasePlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UBaseAbilitySystemComponent* ASC = GetBaseAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void ABasePlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UBaseAbilitySystemComponent* ASC = GetBaseAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

UBaseAbilitySystemComponent* ABasePlayerController::GetBaseAbilitySystemComponent() const
{
	if (const ABasePlayerState* PS = GetPlayerState<ABasePlayerState>())
	{
		return PS->GetBaseAbilitySystemComponent();
	}
	return nullptr;
}
