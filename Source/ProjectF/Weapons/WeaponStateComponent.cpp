// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeaponStateComponent.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffectTypes.h"
#include "Kismet/GameplayStatics.h"
#include "NativeGameplayTags.h"
// TODO: PhysicalMaterialWithTags
// #include "Physics/PhysicalMaterialWithTags.h"
// TODO: TeamSubsystem
// #include "Teams/TeamSubsystem.h"
// TODO: RangedWeaponInstance
// #include "Weapons/RangedWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WeaponStateComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gameplay_Zone, "Gameplay.Zone");

UWeaponStateComponent::UWeaponStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponStateComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (UEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UEquipmentManagerComponent>())
		{
			// TODO: RangedWeaponInstance
			// if (URangedWeaponInstance* CurrentWeapon = Cast<URangedWeaponInstance>(EquipmentManager->GetFirstInstanceOfType(URangedWeaponInstance::StaticClass())))
			// {
			// 	CurrentWeapon->Tick(DeltaTime);
			// }
		}
	}
}

bool UWeaponStateComponent::ShouldShowHitAsSuccess(const FHitResult& Hit) const
{
	AActor* HitActor = Hit.GetActor();

	// TODO: TeamSubsystem
	// //@TODO: Don't treat a hit that dealt no damage (due to invulnerability or similar) as a success
	// UWorld* World = GetWorld();
	// if (UTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UTeamSubsystem>(GetWorld()))
	// {
	// 	return TeamSubsystem->CanCauseDamage(GetController<APlayerController>(), Hit.GetActor());
	// }

	return false;
}

bool UWeaponStateComponent::ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const
{
	//@TODO: Implement me, for the purposes of this component we really only care about damage caused by a weapon
	// or projectile fired from a weapon, and should filter to that
	// (or perhaps see if the causer is also the source of our active reticle config)
	return EffectContext.GetEffectCauser() != nullptr;
}

void UWeaponStateComponent::ClientConfirmTargetData_Implementation(uint16 UniqueId, bool bSuccess, const TArray<uint8>& HitReplaces)
{
	for (int i = 0; i < UnconfirmedServerSideHitMarkers.Num(); i++)
	{
		FServerSideHitMarkerBatch& Batch = UnconfirmedServerSideHitMarkers[i];
		if (Batch.UniqueId == UniqueId)
		{
			if (bSuccess && (HitReplaces.Num() != Batch.Markers.Num()))
			{
				UWorld* World = GetWorld();
				bool bFoundShowAsSuccessHit = false;

				int32 HitLocationIndex = 0;
				for (const FScreenSpaceHitLocation& Entry : Batch.Markers)
				{
					if (!HitReplaces.Contains(HitLocationIndex) && Entry.bShowAsSuccess)
					{
						// Only need to do this once
						if (!bFoundShowAsSuccessHit)
						{
							ActuallyUpdateDamageInstigatedTime();
						}

						bFoundShowAsSuccessHit = true;

						LastWeaponDamageScreenLocations.Add(Entry);
					}
					++HitLocationIndex;
				}
			}

			UnconfirmedServerSideHitMarkers.RemoveAt(i);
			break;
		}
	}
}

void UWeaponStateComponent::AddUnconfirmedServerSideHitMarkers(const FGameplayAbilityTargetDataHandle& InTargetData, const TArray<FHitResult>& FoundHits)
{
	FServerSideHitMarkerBatch& NewUnconfirmedHitMarker = UnconfirmedServerSideHitMarkers.Emplace_GetRef(InTargetData.UniqueId);

	if (APlayerController* OwnerPC = GetController<APlayerController>())
	{
		for (const FHitResult& Hit : FoundHits)
		{
			FVector2D HitScreenLocation;
			if (UGameplayStatics::ProjectWorldToScreen(OwnerPC, Hit.Location, /*out*/ HitScreenLocation, /*bPlayerViewportRelative=*/ false))
			{
				FScreenSpaceHitLocation& Entry = NewUnconfirmedHitMarker.Markers.AddDefaulted_GetRef();
				Entry.Location = HitScreenLocation;
				Entry.bShowAsSuccess = ShouldShowHitAsSuccess(Hit);

				// Determine the hit zone
				FGameplayTag HitZone;

				// TODO: PhysicalMaterialWithTags
				// if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(Hit.PhysMaterial.Get()))
				// {
				// 	for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
				// 	{
				// 		if (MaterialTag.MatchesTag(TAG_Gameplay_Zone))
				// 		{
				// 			Entry.HitZone = MaterialTag;
				// 			break;
				// 		}
				// 	}
				// }
			}
		}
	}
}

void UWeaponStateComponent::UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext)
{
	if (ShouldUpdateDamageInstigatedTime(EffectContext))
	{
		ActuallyUpdateDamageInstigatedTime();
	}
}

void UWeaponStateComponent::ActuallyUpdateDamageInstigatedTime()
{
	// If our LastWeaponDamageInstigatedTime was not very recent, clear our LastWeaponDamageScreenLocations array
	UWorld* World = GetWorld();
	if (World->GetTimeSeconds() - LastWeaponDamageInstigatedTime > 0.1)
	{
		LastWeaponDamageScreenLocations.Reset();
	}
	LastWeaponDamageInstigatedTime = World->GetTimeSeconds();
}

double UWeaponStateComponent::GetTimeSinceLastHitNotification() const
{
	UWorld* World = GetWorld();
	return World->TimeSince(LastWeaponDamageInstigatedTime);
}

