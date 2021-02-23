#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"


UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};


/**
 *	Interface for objects that can be interacted with  
 */
class COOPGAME_API IInteractable
{
	GENERATED_BODY()

public:
	virtual bool IsAvailableForInteraction() const = 0;
	virtual void OnSuccessfulInteraction(APawn* InteractionInstigator) = 0;
};
