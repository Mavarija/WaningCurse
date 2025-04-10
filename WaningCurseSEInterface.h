// Copyright (C) I N F I N I T Y 27 LTD. All rights reserved. 

#pragma once

#include "CoreMinimal.h"
#include "ARSStatisticsComponent.h"
#include "WaningCurseSEInterface.generated.h"

/**
 *
 */
UINTERFACE(Blueprintable)
class SAMSARA_API UWaningCurseSEInterface : public UInterface
{
	GENERATED_BODY()
};


class SAMSARA_API IWaningCurseSEInterface
{
	GENERATED_BODY()

public:
	// Blueprint-implementable function for when a status effect starts
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Waning Curse")
	void HandleWaningCurseStart(FGameplayTag _effectTag);

	// Blueprint-implementable function for when a status effect ends
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Waning Curse")
	void HandleWaningCurseEnd(FGameplayTag _effectTag);
};