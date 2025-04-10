// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WaningCurseStatusEffect.h"

#include "WaningCurseManager.generated.h"

// Delegate to let WaningCurse know when manager is destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnManagerDestroyed);

UCLASS()
class SAMSARA_API AWaningCurseManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWaningCurseManager();

	UPROPERTY(EditAnywhere, Category = "WaningCurse")
	FGameplayTag waningCurseTag;

	UPROPERTY(BlueprintAssignable, Category = "WaningCurse")
	FOnManagerDestroyed OnManagerDestroyed;

	// Called when first spawned (essentially BeginPlay)
	UFUNCTION()
	void InitializeManager(AActor* _target);

	// Manager removal - On StatusEffectEnd
	UFUNCTION()
	void OnStatusEffectRemoved(FGameplayTag _statusEffectTag);

	// Manager removal - On HealthDepleated
	UFUNCTION()
	void OnTargetDied(FGameplayTag _statTag);

private:
	// The target actor for the spell effect
	UPROPERTY()
	TObjectPtr<AActor> targetActor;

	// Status effect class loaded from soft reference
	UPROPERTY(EditDefaultsOnly, Category = "WaningCurse")
	TSoftClassPtr<UWaningCurseStatusEffect> statusEffectClass;

	// Active instance of the status effect
	UPROPERTY()
	TObjectPtr<UWaningCurseStatusEffect> statusEffectObj;

	// Tracks if status effect is active
	bool bIsStatusEffectOn;

};
