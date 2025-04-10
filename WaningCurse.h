// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#pragma once

#include "CoreMinimal.h"
#include "../SamsaraBaseSpell_cpp.h"
#include "WaningCurseManager.h"
#include "WaningCurse.generated.h"

/**
 *
 */

UCLASS()
class SAMSARA_API UWaningCurse : public USamsaraBaseSpell_cpp
{
	GENERATED_BODY()

public:
	virtual void OnActionStarted_Implementation(const FString& _contextString = "") override;

	// Delegate function to clear the manager pointer
	UFUNCTION()
	void OnManagerDeleted();

protected:
	// Refund function
	void RefundSpellCost();

	// The class type of the WaningCurseManager to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaningCurse")
	TSubclassOf<AWaningCurseManager> managerClass;

	// Pointer to the currently active WaningCurseManager
	UPROPERTY()
	TObjectPtr<AWaningCurseManager> manager;

private:

};
