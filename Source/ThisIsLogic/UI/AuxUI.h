#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "UObject/ScriptDelegates.h"

class FAuxUI
{
	public:

		FAuxUI() = delete;

		static void AssignButtonClick(
			UObject* Listener,
			UButton* Button,
			FName FunctionName
		)
		{
			FScriptDelegate ClickDelegate;

			ClickDelegate.
				BindUFunction(Listener, FunctionName);

			FOnButtonClickedEvent& ButtonClicked =
				Button->OnClicked;

			ButtonClicked.Add(ClickDelegate);
		}
};