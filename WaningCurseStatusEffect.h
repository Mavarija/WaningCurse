// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#pragma once

#include "CoreMinimal.h"
#include "StatusEffects/ACFDamageOverTimeStatusEffect.h"
#include "ARSStatisticsComponent.h"
#include "Curves/CurveFloat.h"
#include "WaningCurseStatusEffect.generated.h"

/**
 *
 */
UCLASS()
class SAMSARA_API UWaningCurseStatusEffect : public UACFDamageOverTimeStatusEffect
{
	GENERATED_BODY()

public:

protected:
	virtual void OnStatusEffectStarts_Implementation(ACharacter* Character) override;

	virtual void OnTriggerStatusEffect_Implementation() override;

	virtual void OnStatusEffectEnds_Implementation() override;


	// To use with the OnStatisticReachesZero
	UFUNCTION()
	void OnTargetHealthDepleted(FGameplayTag _statTag);


	// Curve to determine damage scaling based on Wisdom
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* wisdomMultiplierCurve;

	// Reference to the statistics component for accessing character attributes
	UPROPERTY()
	UARSStatisticsComponent* statComp;

	// Toggle material overlay in editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaningCurse")
	bool bApplyMaterialOverlay = true;

private:
	// Base damage value before scaling
	UPROPERTY(EditAnywhere, Category = "WaningCurse")
	float baseDamage;

	// Material used for visualizing the curse effect
	UPROPERTY(EditAnywhere, Category = "WaningCurse")
	UMaterialInterface* curseMaterial;

	// Particles
	UPROPERTY(EditAnywhere, Category = "WaningCurse")
	UParticleSystem* curseParticle;
	// Component
	UPROPERTY()
	UParticleSystemComponent* activeEnemyVFX;
	UPROPERTY()
	UParticleSystemComponent* activePlayerVFX;

};
