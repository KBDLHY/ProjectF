// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cosmetics/ControllerComponent_CharacterParts.h"
#include "Cosmetics/CharacterPartTypes.h"
#include "Cosmetics/PawnComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "CosmeticDeveloperSettings.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ControllerComponent_CharacterParts)

//////////////////////////////////////////////////////////////////////

UControllerComponent_CharacterParts::UControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	// Listen for pawn possession changed events
	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = GetPawn<APawn>())
			{
				OnPossessedPawnChanged(nullptr, ControlledPawn);
			}
		}

		ApplyDeveloperSettings();
	}
}

void UControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

UPawnComponent_CharacterParts* UControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UPawnComponent_CharacterParts>();
	}
	return nullptr;
}

void UControllerComponent_CharacterParts::AddCharacterPart(const FCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart, ECharacterPartSource::Natural);
}

void UControllerComponent_CharacterParts::AddCharacterPartInternal(const FCharacterPart& NewPart, ECharacterPartSource Source)
{
	FControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;
	NewEntry.Source = Source;

	if (UPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		if (NewEntry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
		{
			NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		}
	}

}

void UControllerComponent_CharacterParts::RemoveCharacterPart(const FCharacterPart& PartToRemove)
{
	for (auto EntryIt = CharacterParts.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (FCharacterPart::AreEquivalentParts(EntryIt->Part, PartToRemove))
		{
			if (UPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
			{
				PawnCustomizer->RemoveCharacterPart(EntryIt->Handle);
			}

			EntryIt.RemoveCurrent();
			break;
		}
	}
}

void UControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}

	CharacterParts.Reset();
}

void UControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Remove from the old pawn
	if (UPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UPawnComponent_CharacterParts>() : nullptr)
	{
		for (FControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// Apply to the new pawn
	if (UPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UPawnComponent_CharacterParts>() : nullptr)
	{
		for (FControllerCharacterPartEntry& Entry : CharacterParts)
		{
			// Don't readd if it's already there, this can get called with a null oldpawn
			if (!Entry.Handle.IsValid() && Entry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
			{
				Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
			}
		}
	}
}

void UControllerComponent_CharacterParts::ApplyDeveloperSettings()
{
#if UE_WITH_CHEAT_MANAGER
	// TODO: UCosmeticDeveloperSettings
	// const UCosmeticDeveloperSettings* Settings = GetDefault<UCosmeticDeveloperSettings>();
	//
	// // Suppress or unsuppress natural parts if needed
	// const bool bSuppressNaturalParts = (Settings->CheatMode == ECosmeticCheatMode::ReplaceParts) && (Settings->CheatCosmeticCharacterParts.Num() > 0);
	// SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	//
	// // Remove anything added by developer settings and re-add it
	// UPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	// for (auto It = CharacterParts.CreateIterator(); It; ++It)
	// {
	// 	if (It->Source == ECharacterPartSource::AppliedViaDeveloperSettingsCheat)
	// 	{
	// 		if (PawnCustomizer != nullptr)
	// 		{
	// 			PawnCustomizer->RemoveCharacterPart(It->Handle);
	// 		}
	// 		It.RemoveCurrent();
	// 	}
	// }
	//
	// // Add new parts
	// for (const FCharacterPart& PartDesc : Settings->CheatCosmeticCharacterParts)
	// {
	// 	AddCharacterPartInternal(PartDesc, ECharacterPartSource::AppliedViaDeveloperSettingsCheat);
	// }
#endif
}


void UControllerComponent_CharacterParts::AddCheatPart(const FCharacterPart& NewPart, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	AddCharacterPartInternal(NewPart, ECharacterPartSource::AppliedViaCheatManager);
#endif
}

void UControllerComponent_CharacterParts::ClearCheatParts()
{
#if UE_WITH_CHEAT_MANAGER
	UPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	// Remove anything added by cheat manager cheats
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaCheatManager)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	ApplyDeveloperSettings();
#endif
}

void UControllerComponent_CharacterParts::SetSuppressionOnNaturalParts(bool bSuppressed)
{
#if UE_WITH_CHEAT_MANAGER
	UPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	for (FControllerCharacterPartEntry& Entry : CharacterParts)
	{
		if ((Entry.Source == ECharacterPartSource::Natural) && bSuppressed)
		{
			// Suppress
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(Entry.Handle);
				Entry.Handle.Reset();
			}
			Entry.Source = ECharacterPartSource::NaturalSuppressedViaCheat;
		}
		else if ((Entry.Source == ECharacterPartSource::NaturalSuppressedViaCheat) && !bSuppressed)
		{
			// Unsuppress
			if (PawnCustomizer != nullptr)
			{
				Entry.Handle = PawnCustomizer->AddCharacterPart(Entry.Part);
			}
			Entry.Source = ECharacterPartSource::Natural;
		}
	}
#endif
}

