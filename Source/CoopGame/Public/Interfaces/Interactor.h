#pragma once

#include "Interactable.h"

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactor.generated.h"

UINTERFACE()
class UInteractor : public UInterface
{
	GENERATED_BODY()
};


/**
*	Interface for objects (usually Pawns) that can be interact with interactables  
*/
class COOPGAME_API IInteractor
{
	GENERATED_BODY()

public:
	virtual bool WantToInteract(IInteractable* Interactable) const = 0;
	virtual void Interact(IInteractable* Interactable) = 0;
};
