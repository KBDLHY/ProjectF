// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ProjectF/BaseGameplayTags.h"
#include "ProjectF/AbilitySystem/BaseAbilitySystemComponent.h"
#include "ProjectF/AbilitySystem/Abilities/Ability_CharacterCrouch.h"
#include "ProjectF/AbilitySystem/Abilities/Ability_CharacterJump.h"
#include "ProjectF/AbilitySystem/Abilities/Ability_CharacterRun.h"
#include "ProjectF/Player/BasePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseCharacter)

ABaseCharacter::ABaseCharacter()
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	BaseEyeHeight = 80.f;
	CrouchedEyeHeight = 50.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// CharacterMovement (General Settings)
	GetCharacterMovement()->GravityScale = 1.f;
	GetCharacterMovement()->MaxAcceleration = 2400.f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	GetCharacterMovement()->BrakingFriction = 6.f;
	GetCharacterMovement()->SetCrouchedHalfHeight(65.f);
	// CharacterMovement: Walking
	GetCharacterMovement()->GroundFriction = 8.f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	// CharacterMovement: Rotation Settings
	GetCharacterMovement()->RotationRate.Yaw = 560.f;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// CharacterMovement: RootMotion
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	// CharacterMovement: NavMovement
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArmComponent->TargetArmLength = 450.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetRelativeLocation(FVector(0.f, 70.f, 0.f));
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
		FGameplayAbilitySpec JumpSpec(UAbility_CharacterJump::StaticClass());
		JumpSpec.GetDynamicSpecSourceTags().AddTag(BaseGameplayTags::Input_Jump);
		AbilitySystemComponent->GiveAbility(JumpSpec);

		FGameplayAbilitySpec CrouchSpec(UAbility_CharacterCrouch::StaticClass());
		CrouchSpec.GetDynamicSpecSourceTags().AddTag(BaseGameplayTags::Input_Crouch);
		AbilitySystemComponent->GiveAbility(CrouchSpec);

		FGameplayAbilitySpec RunSpec(UAbility_CharacterRun::StaticClass());
		RunSpec.GetDynamicSpecSourceTags().AddTag(BaseGameplayTags::Input_Run);
		AbilitySystemComponent->GiveAbility(RunSpec);
	}
}

