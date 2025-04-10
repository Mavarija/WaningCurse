// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#include "WaningCurse.h"


void UWaningCurse::OnActionStarted_Implementation(const FString& _contextString)
{
	Super::OnActionStarted_Implementation(_contextString);

	// Get target
	TObjectPtr<AActor> targetActor = GetSpellTargetWithinRange(CharacterOwner);
	if (!targetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaningCurse: No Target Found"));

		// refund 
		RefundSpellCost();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("WaningCurse: Yay Target!"));

	// Check if manager already exists
	if (manager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaningCurse: Manager Already Exists at Memory Address: %p"), &manager);
		// refund
		RefundSpellCost();
		return;
	}

	// Spawn the manager 
	TObjectPtr<AActor> spawnedManager = GetWorld()->SpawnActor(managerClass);
	if (!spawnedManager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaningCurse: Failed to spawn WaningCurseManager!"));

		// refund
		RefundSpellCost();
		return;
	}
	// Cast to the correct manager class
	manager = Cast<AWaningCurseManager>(spawnedManager);
	if (!manager)
	{
		UE_LOG(LogTemp, Error, TEXT("WaningCurse: Failed to cast spawned actor to AWaningCurseManager!"));

		// refund
		RefundSpellCost();
		return;
	}

	// Manager destruction delegate call
	manager->OnManagerDestroyed.AddDynamic(this, &UWaningCurse::OnManagerDeleted);

	// Accessing the manager
	manager->InitializeManager(targetActor);

}

void UWaningCurse::OnManagerDeleted()
{
	UE_LOG(LogTemp, Warning, TEXT("WaningCurse: Manager was deleted, resetting pointer."));
	if (manager)
	{
		// unbind delegate
		manager->OnManagerDestroyed.RemoveDynamic(this, &UWaningCurse::OnManagerDeleted);
		// prevent stale pointer issues
		manager = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("WaningCurse: Manager pointer reset to nullptr."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaningCurse: OnManagerDeleted() called, but manager was already nullptr!"));
	}
}

void UWaningCurse::RefundSpellCost()
{
	UACFEquipmentComponent* equipmentComponent = CharacterOwner->GetComponentByClass<UACFEquipmentComponent>();
	if (equipmentComponent)
	{
		for (FBaseItem itemToRefund : spellInfo->itemsToConsume)
		{
			equipmentComponent->AddItemToInventory(itemToRefund);
		}
	}
}
