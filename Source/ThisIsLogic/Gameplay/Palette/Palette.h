#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../../Logic/LogicTypes.h"
#include "../Block/AbstractBlocksManager.h"

#include "Palette.generated.h"

DECLARE_DELEGATE_OneParam(FOnPaletteInsertRequested, const FToken&);

DECLARE_DELEGATE_OneParam(
	FOnPaletteInsertAfterRequested, const FToken&
);

UCLASS(Abstract)
class THISISLOGIC_API APalette : public AAbstractBlocksManager
{
	GENERATED_BODY()

	public:

		template<typename UserClass>
		void SetInsertRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(const FToken&)
		)
		{
			this->OnInsertRequested.BindUObject(Listener, Callback);
		}
	
		template<typename UserClass>
		void SetInsertAfterRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(const FToken&)
		)
		{
			this->OnInsertAfterRequested.
				BindUObject(Listener, Callback);
		}

	#pragma region ProtectedMethods
	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			double MaxRowWidth = this->GetMaxRowWidth();

			this->SetMaxRowWidth(MaxRowWidth);

			TArray<FToken> Tokens = this->GetItemTokens();

			this->Board->CreateBlocks(Tokens);
		}

		virtual TArray<FToken> GetItemTokens() const
		{
			unimplemented();

			return TArray<FToken>();
		}

		virtual double GetMaxRowWidth() const
		{
			unimplemented();

			return 0.0;
		}

		virtual double GetRowHeightRatio() const override
		{
			return APalette::RowHeightRatio;
		}

		virtual void OnBlockTapped(ABlock* Block) override
		{
			this->RequestInsert(Block);
		}

		virtual void OnBlockLongPressed(ABlock* Block)
			override
		{
			this->RequestInsertAfter(Block);
		}
	
	#pragma endregion
	
	private:

		void RequestInsert(ABlock* Block)
		{
			bool bHasInsertListener =
				this->OnInsertRequested.IsBound();

			check(bHasInsertListener);

			FToken Token = Block->GetToken();

			this->OnInsertRequested.Execute(Token);
		}

		void RequestInsertAfter(ABlock* Block)
		{
			bool bHasInsertAfterListener =
				this->OnInsertAfterRequested.IsBound();

			check(bHasInsertAfterListener);

			FToken Token = Block->GetToken();

			this->OnInsertAfterRequested.Execute(Token);
		}

		FOnPaletteInsertRequested OnInsertRequested;

		FOnPaletteInsertAfterRequested OnInsertAfterRequested;

		static constexpr double RowHeightRatio = 1.1;
};