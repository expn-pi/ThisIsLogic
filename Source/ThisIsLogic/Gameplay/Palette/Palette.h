#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../../Logic/LogicTypes.h"
#include "../Block/AbstractBlocksManager.h"

#include "Palette.generated.h"

DECLARE_DELEGATE_OneParam(FOnPaletteInsertRequested, const FToken&);

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

		virtual void OnBlockSelectRequested(ABlock* Block) override
		{
			bool bHasInsertListener =
				this->OnInsertRequested.IsBound();

			check(bHasInsertListener);

			FToken Token = Block->GetToken();

			this->OnInsertRequested.Execute(Token);
		}

	private:

		FOnPaletteInsertRequested OnInsertRequested;

		static constexpr double RowHeightRatio = 1.1;
};