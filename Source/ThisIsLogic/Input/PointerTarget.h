#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "PointerTarget.generated.h"

UINTERFACE(
	MinimalAPI, meta =
		(CannotImplementInterfaceInBlueprint)
)
class UPointerTarget : public UInterface
{
	GENERATED_BODY()
};

class THISISLOGIC_API IPointerTarget
{
	GENERATED_BODY()

	public:

		virtual void PointerPressed(const FVector& Point) = 0;

		virtual void PointerHeld(const FVector& Point) = 0;

		virtual void PointerReleased() = 0;
};