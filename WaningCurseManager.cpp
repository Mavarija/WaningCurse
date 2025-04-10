// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#include "WaningCurseManager.h"
#include "Components/ACFStatusEffectManagerComponent.h"
#include "WaningCurseStatusEffect.h"

// Sets default values
AWaningCurseManager::AWaningCurseManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsStatusEffectOn = false;
}

// Called when created by the WaningCurse spell
void AWaningCurseManager::InitializeManager(AActor* _target)
{
	UE_LOG(LogTemp, Warning, TEXT("Hellooooo, Manager Here o/"));

	// Set the target
	if (!_target)
	{
		UE_LOG(LogTemp, Error, TEXT("Manager: Target actor is NULL!"));
		return;
	}
	targetActor = _target;

	// Check if there is an already active Status Effect
	if (bIsStatusEffectOn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Manager: Status Effect Already Active!"));
		return;
	}

	// Load status effect class
	if (!statusEffectClass.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Manager: statusEffectClass is not set!"));
		return;
	}
	statusEffectClass = statusEffectClass.LoadSynchronous();
	if (!statusEffectClass.Get())
	{
		UE_LOG(LogTemp, Error, TEXT("Manager: Failed to load status effect class!"));
		return;
	}

	// Create status effect manager component
	UACFStatusEffectManagerComponent* statusEffectManagerComp = targetActor->GetComponentByClass<UACFStatusEffectManagerComponent>();
	if (!statusEffectManagerComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Manager: Cannot find the status manager component on target!"));
		return;
	}
	// Apply the status effect
	statusEffectManagerComp->CreateAndApplyStatusEffect(statusEffectClass.Get());
	// Set the status effect as active
	bIsStatusEffectOn = true;
	UE_LOG(LogTemp, Warning, TEXT("Manager: Status Effect Applied!"));

	// Bind to the OnStatusRemoved event
	statusEffectManagerComp->OnStatusRemoved.AddDynamic(this, &AWaningCurseManager::OnStatusEffectRemoved);

	// Remove manager on target death
	if (targetActor)
	{
		UARSStatisticsComponent* targetStats = targetActor->FindComponentByClass<UARSStatisticsComponent>();
		if (targetStats)
		{
			targetStats->OnStatisiticReachesZero.AddDynamic(this, &AWaningCurseManager::OnTargetDied);
		}
	}

}

// Destroy manager when the Status Effect ends
void AWaningCurseManager::OnStatusEffectRemoved(FGameplayTag _statusEffectTag)
{
	UE_LOG(LogTemp, Warning, TEXT("Manager: OnStatusEffectRemoved CALLED!"));

	if (_statusEffectTag == waningCurseTag)
	{
		// set effet as off
		bIsStatusEffectOn = false;

		// notify WaningCurse spell class
		OnManagerDestroyed.Broadcast();

		// Clear any lingering references
		//OnManagerDestroyed.Clear();

		// Unbind Status Effect Removed
		if (UACFStatusEffectManagerComponent* statusEffectManagerComp = targetActor->FindComponentByClass<UACFStatusEffectManagerComponent>())
		{
			statusEffectManagerComp->OnStatusRemoved.RemoveDynamic(this, &AWaningCurseManager::OnStatusEffectRemoved);
		}

		// destroy manager
		UE_LOG(LogTemp, Warning, TEXT("Manager: Bye Bye!"));
		GetWorld()->DestroyActor(this);

	}

}

// Destroy manager when the Enemy Dies
void AWaningCurseManager::OnTargetDied(FGameplayTag _statTag)
{
	UE_LOG(LogTemp, Warning, TEXT("Manager: Enemy died! Forcing self-destruction."));

	if (_statTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("RPG.Statistics.Health"))))
	{
		OnManagerDestroyed.Broadcast();

		// Unbind OnTargetDied
		if (UARSStatisticsComponent* targetStats = targetActor->FindComponentByClass<UARSStatisticsComponent>())
		{
			targetStats->OnStatisiticReachesZero.RemoveDynamic(this, &AWaningCurseManager::OnTargetDied);
		}

		// destroy manager
		UE_LOG(LogTemp, Warning, TEXT("Manager: Bye Bye! 2.0"));
		GetWorld()->DestroyActor(this);
	}

}
