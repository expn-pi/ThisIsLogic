#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "../../Input/PointerTarget.h"

#include "Block.generated.h"

class ABlock;

DECLARE_DELEGATE_OneParam(FOnBlockWidthChanged, ABlock*);

DECLARE_DELEGATE_TwoParams(
	FOnBlockMoveRequested, ABlock*, const FVector&
);

DECLARE_DELEGATE_OneParam(FOnBlockDropRequested, ABlock*);

UCLASS()
class THISISLOGIC_API ABlock :
	public AActor, public IPointerTarget
{
	GENERATED_BODY()

	public:

		ABlock()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName MeshName = TEXT("Mesh");

			this->Mesh =
				CreateDefaultSubobject<
					UProceduralMeshComponent
				>(MeshName);

			this->RootComponent = this->Mesh;
		}

		virtual void OnConstruction(
			const FTransform& Transform
		)
			override
		{
			Super::OnConstruction(Transform);

			this->BuildMesh();
			this->ApplyMaterial();
		}

		float GetWidth() const
		{
			return this->Width;
		}

		template<typename UserClass>
		void SetWidthChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnWidthChanged.
				BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetMoveRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(
				ABlock*, const FVector&
			)
		)
		{
			this->OnMoveRequested.
				BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetDropRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnDropRequested.
				BindUObject(Listener, Callback);
		}

		virtual void PointerPressed(
			const FVector& Point
		)
			override
		{
			FVector Location = this->GetActorLocation();

			this->GrabOffset = Location - Point;
		}

		virtual void PointerHeld(const FVector& Point)
			override
		{
			bool bHasMoveListener =
				this->OnMoveRequested.IsBound();

			check(bHasMoveListener);

			FVector DesiredLocation =
				Point + this->GrabOffset;

			this->OnMoveRequested.
				Execute(this, DesiredLocation);
		}

		virtual void PointerReleased() override
		{
			bool bHasDropListener =
				this->OnDropRequested.IsBound();

			check(bHasDropListener);

			this->OnDropRequested.Execute(this);
		}

	private:

		void BuildMesh()
		{
			TArray<FVector> Vertices = this->GetVertices();

			TArray<int32> Triangles = {
				0, 1, 2,
				0, 2, 3
			};

			FVector Up = FVector::UpVector;

			TArray<FVector> Normals;
			Normals.Init(Up, 4);

			TArray<FVector2D> UVs;
			TArray<FLinearColor> VertexColors;
			TArray<FProcMeshTangent> Tangents;

			this->Mesh->
				CreateMeshSection_LinearColor(
					0,
					Vertices,
					Triangles,
					Normals,
					UVs,
					VertexColors,
					Tangents,
					true
				);
		}

		void SetWidth(float NewWidth)
		{
			this->Width = NewWidth;

			this->UpdateMeshVertices();

			this->OnWidthChanged.Execute(this);
		}

		void UpdateMeshVertices()
		{
			TArray<FVector> Vertices = this->GetVertices();

			TArray<FVector> Normals;
			TArray<FVector2D> UVs;
			TArray<FLinearColor> VertexColors;
			TArray<FProcMeshTangent> Tangents;

			this->Mesh->
				UpdateMeshSection_LinearColor(
					0,
					Vertices,
					Normals,
					UVs,
					VertexColors,
					Tangents
				);
		}

		TArray<FVector> GetVertices() const
		{
			float HalfWidth = this->Width * 0.5f;
			float HalfHeight = this->Height * 0.5f;

			FVector BottomLeft =
				FVector(-HalfHeight, -HalfWidth, 0.f);

			FVector BottomRight =
				FVector(-HalfHeight, HalfWidth, 0.f);

			FVector TopRight =
				FVector(HalfHeight, HalfWidth, 0.f);

			FVector TopLeft =
				FVector(HalfHeight, -HalfWidth, 0.f);

			TArray<FVector> Vertices;
			Vertices.Add(BottomLeft);
			Vertices.Add(BottomRight);
			Vertices.Add(TopRight);
			Vertices.Add(TopLeft);

			return Vertices;
		}

		void ApplyMaterial()
		{
			bool bHasMaterial = this->Material != nullptr;

			if (bHasMaterial)
			{
				UMaterialInstanceDynamic* DynamicMaterial =
					UMaterialInstanceDynamic::
					Create(this->Material, this);

				FName ColorParameterName =
					TEXT("Color");

				DynamicMaterial->
					SetVectorParameterValue(
						ColorParameterName,
						this->Color
					);

				this->Mesh->
					SetMaterial(0, DynamicMaterial);
			}
		}

		FOnBlockWidthChanged OnWidthChanged;

		FOnBlockMoveRequested OnMoveRequested;

		FOnBlockDropRequested OnDropRequested;

		FVector GrabOffset = FVector::ZeroVector;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UProceduralMeshComponent> Mesh;

		UPROPERTY(EditAnywhere, Category = "Block")
		TObjectPtr<UMaterialInterface> Material;

		UPROPERTY(EditAnywhere, Category = "Block")
		FLinearColor Color = FLinearColor::Blue;

		UPROPERTY(EditAnywhere, Category = "Block")
		float Width = 100.f;

		UPROPERTY(EditAnywhere, Category = "Block")
		float Height = 100.f;
};