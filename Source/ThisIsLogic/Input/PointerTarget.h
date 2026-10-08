#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "PointerTarget.generated.h"

UINTERFACE(
	MinimalAPI,
	meta = (
		CannotImplementInterfaceInBlueprint
	)
)
class UPointerTarget : public UInterface
{
	GENERATED_BODY()
};

class THISISLOGIC_API IPointerTarget
{
	GENERATED_BODY()

	public:

		virtual void Tapped() = 0;

		virtual void
			DragStarted(
				const FVector& PressPoint
			) = 0;

		virtual void
			Dragged(
				const FVector& Point
			) = 0;

		virtual void DragEnded() = 0;
};