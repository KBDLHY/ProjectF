// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

struct FServerParameters;
struct FProcessParameters;

UCLASS()
class PROJECTF_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABaseGameMode();

protected:
	virtual void BeginPlay() override;

private:
	TSharedPtr<FProcessParameters> ProcessParameters;

	void InitGameLift();
	void SetServerParameters(bool& bIsAnywhereActive, FServerParameters& OutServerParameters);
	void ParseCommandLinePort(int32& OutPort);
};
