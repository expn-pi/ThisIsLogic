#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Block.generated.h"

class ABlock;

DECLARE_MULTICAST_DELEGATE(FOnBlockWidthChanged);

UCLASS()
class THISISLOGIC_API ABlock : public AActor
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
		void AddWidthChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnWidthChanged.
				AddUObject(Listener, Callback);
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

			this->OnWidthChanged.Broadcast();
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
			bool bHasMaterial =
				this->Material != nullptr;

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