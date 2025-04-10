// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#include "WaningCurseStatusEffect.h"
#include "Samsara/DamageTypes/VoidDamageType.h"
#include "CCMFadeableActorComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include "Kismet/KismetMathLibrary.h"
#include <NiagaraFunctionLibrary.h>
#include "WaningCurseSEInterface.h"
#include "Components/AudioComponent.h"

// () Parenthesis
// [] Bracket
// {} Brace
// <> Chevron

void UWaningCurseStatusEffect::OnStatusEffectStarts_Implementation(ACharacter* _character)
{
	Super::OnStatusEffectStarts_Implementation(_character);

	UE_LOG(LogTemp, Warning, TEXT("StatusEffect: OnStatusEffectStart called on %s"), *_character->GetName());
	CharacterOwner = _character; // enemy who got cursed
	if (!CharacterOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: CharacterOwner is NULL after assignment in OnStatusEffectStarts!"));
	}

	// Getting and checking player pawn
	ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("StatusEffect: Player/Caster is %s"), *PlayerCharacter->GetName());

		// Interface on status start implementation
		if (PlayerCharacter->GetClass()->ImplementsInterface(UWaningCurseSEInterface::StaticClass()))
		{
			IWaningCurseSEInterface::Execute_HandleWaningCurseStart(PlayerCharacter, StatusEffectTag);
		}
		// Statistics component check for wisdom scaling
		statComp = PlayerCharacter->GetComponentByClass<UARSStatisticsComponent>();
		if (!statComp)
		{
			UE_LOG(LogTemp, Error, TEXT("StatusEffect: No UARSStatisticsComponent found on %s!"), *PlayerCharacter->GetName());
		}

		// Spawn particle effect on Player
		if (curseParticle && PlayerCharacter->GetMesh())
		{
			FName playerSocket = TEXT("neck_01");
			activePlayerVFX = UGameplayStatics::SpawnEmitterAttached(
				curseParticle,
				PlayerCharacter->GetMesh(),			// Attach to skeletal mesh
				playerSocket,						// To this socket
				FVector(0, 0, 0),					// Local offset
				FRotator(0, 0, 0),					// No rotation offset
				EAttachLocation::SnapToTarget);		// Snap to socket position
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: Player Pawn is NULL!"));
	}

	if (CharacterOwner)
	{
		// Get the statistics component from the affected enemy (Character Owner)
		UARSStatisticsComponent* TargetStatsComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
		if (TargetStatsComp)
		{
			// Bind to OnStatisticReachesZero to detect when health reaches zero
			TargetStatsComp->OnStatisiticReachesZero.AddDynamic(this, &UWaningCurseStatusEffect::OnTargetHealthDepleted);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("StatusEffect: No UARSStatisticsComponent found on %s!"), *CharacterOwner->GetName());
		}

		// Spawn particle effect on Enemy
		if (curseParticle && CharacterOwner->GetMesh())
		{
			FName enemySocket = TEXT("neck_01");
			activeEnemyVFX = UGameplayStatics::SpawnEmitterAttached(
				curseParticle,
				CharacterOwner->GetMesh(),
				enemySocket,
				FVector(0, 0, 0),
				FRotator(0, 0, 0),
				EAttachLocation::SnapToTarget);
		}

	}

}

void UWaningCurseStatusEffect::OnTriggerStatusEffect_Implementation()
{
	Super::OnTriggerStatusEffect_Implementation();

	// Check for statComp
	if (!statComp)
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: statComp is NULL! Skipping damage calculation."));
		return;
	}
	// Geting player wisdom
	FGameplayTag wisdomTag = FGameplayTag::RequestGameplayTag(FName("RPG.PrimaryAttributes.Wisdom"));
	float characterWisdom = statComp->GetCurrentPrimaryAttributeValue(wisdomTag);
	// Damage scaling on wisdom by curve
	float curveMultiplier = 0.5;
	if (wisdomMultiplierCurve)
	{
		curveMultiplier = wisdomMultiplierCurve->GetFloatValue(characterWisdom);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: wisdomMultiplierCurve is NULL!"));
	}
	// Scaled damage calculation
	float scaledDmg = curveMultiplier * baseDamage;

	// Check for CharacterOwner
	if (!CharacterOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: CharacterOwner is NULL in OnTriggerStatusEffect!"));
		return;
	}
	// Apply damage to status effect's owner
	// UGameplayStatics::ApplyDamage(CharacterOwner, scaledDmg, nullptr, nullptr, UVoidDamageType::StaticClass());
	UGameplayStatics::ApplyDamage(CharacterOwner, scaledDmg,
		UGameplayStatics::GetPlayerController(GetCharacterOwner()->GetWorld(), 0),
		UGameplayStatics::GetPlayerPawn(GetCharacterOwner()->GetWorld(), 0),
		UVoidDamageType::StaticClass());

	if (bApplyMaterialOverlay)
	{
		// Create enemy material overlay
		UCCMFadeableActorComponent* materialComp = nullptr;
		if (CharacterOwner)
		{
			materialComp = CharacterOwner->FindComponentByClass<UCCMFadeableActorComponent>();
		}
		if (materialComp)
		{
			// Apply enemy material overlay
			materialComp->SetMaterialOverlay(curseMaterial);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StatusEffect: No UCCMFadeableActorComponent found on target!"));
		}
	}

}

void UWaningCurseStatusEffect::OnStatusEffectEnds_Implementation()
{
	Super::OnStatusEffectEnds_Implementation();

	ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (PlayerCharacter)
	{
		// Interface on status end
		if (PlayerCharacter->GetClass()->ImplementsInterface(UWaningCurseSEInterface::StaticClass()))
		{
			IWaningCurseSEInterface::Execute_HandleWaningCurseEnd(PlayerCharacter, StatusEffectTag);

			UE_LOG(LogTemp, Warning, TEXT("OnStatusEffectEnds: INTERFACE CALLED TO END THE EFFECT!"));

		}

		// Remove Player particle effect
		if (activePlayerVFX)
		{
			activePlayerVFX->Deactivate();
			//activePlayerVFX->DestroyComponent();
			activePlayerVFX = nullptr;
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: Player Pawn is NULL!"));
	}


	// Unbind the OnStatisticReachesZero health event (avoid dangling references)
	if (CharacterOwner)
	{
		UARSStatisticsComponent* TargetStatsComp = CharacterOwner->FindComponentByClass<UARSStatisticsComponent>();
		if (TargetStatsComp)
		{
			TargetStatsComp->OnStatisiticReachesZero.RemoveDynamic(this, &UWaningCurseStatusEffect::OnTargetHealthDepleted);
			UE_LOG(LogTemp, Warning, TEXT("StatusEffect: Unbound HealthDepleted event."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("StatusEffect: Could not find UARSStatisticsComponent on %s!"), *CharacterOwner->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StatusEffect: CharacterOwner is NULL! Could not unbind event."));
	}


	// Remove Enemy material overlay
	UCCMFadeableActorComponent* materialComp = CharacterOwner->FindComponentByClass<UCCMFadeableActorComponent>();
	if (materialComp)
	{
		materialComp->RemoveMaterialOverlay();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StatusEffect: No materialComp found on target!"));
	}
	// Remove Enemy particle effects
	if (activeEnemyVFX)
	{
		activeEnemyVFX->Deactivate();
		//activeEnemyVFX->DestroyComponent();
		activeEnemyVFX = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StatusEffect: No particleComp found on target!"));
	}

}


void UWaningCurseStatusEffect::OnTargetHealthDepleted(FGameplayTag _statTag)
{
	// Check if the health stat has reached zero
	if (_statTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("RPG.Statistics.Health"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("StatusEffect: Target Died, Removing Effect!"));

		// Call to remove the effect + particles
		OnStatusEffectEnds_Implementation();
	}

}

