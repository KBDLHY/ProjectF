// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "ProjectF/Character/BaseCharacter.h"
#include "ProjectF/Player/BasePlayerController.h"
#include "ProjectF/Player/BasePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseGameMode)

ABaseGameMode::ABaseGameMode()
{
	PlayerControllerClass = ABasePlayerController::StaticClass();
	PlayerStateClass = ABasePlayerState::StaticClass();
	DefaultPawnClass = ABaseCharacter::StaticClass();
}
