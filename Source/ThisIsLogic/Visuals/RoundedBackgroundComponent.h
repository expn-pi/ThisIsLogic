#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "RoundedBackgroundComponent.generated.h"

UCLASS()
class THISISLOGIC_API URoundedBackgroundComponent :
	public UProceduralMeshComponent
{
	GENERATED_BODY()

	public:

		URoundedBackgroundComponent(
			const FObjectInitializer& ObjectInitializer
		)
			: Super(ObjectInitializer)
		{
			FVector BottomLeft = FVector(-1.f, -1.f, 0.f);
			FVector BottomRight = FVector(-1.f, 1.f, 0.f);
			FVector TopRight = FVector(1.f, 1.f, 0.f);
			FVector TopLeft = FVector(1.f, -1.f, 0.f);

			this->CornerSigns.Add(BottomLeft);
			this->CornerSigns.Add(BottomRight);
			this->CornerSigns.Add(TopRight);
			this->CornerSigns.Add(TopLeft);
		}

		void SetColor(
			const FLinearColor& NewColor
		)
		{
			this->Color = NewColor;
		}

		void SetBaseMaterial(
			UMaterialInterface* NewBaseMaterial
		)
		{
			this->BaseMaterial = NewBaseMaterial;
		}

		void SetRoundness(float NewRoundness)
		{
			this->Roundness = NewRoundness;
		}

		void Build(float Width, float Height)
		{
			this->BuildCornerOffsets(Width, Height);
			this->BuildVertices(Width, Height);
			this->BuildMesh();
			this->ApplyMaterial();
		}

		void Resize(float Width, float Height)
		{
			this->BuildCornerOffsets(Width, Height);
			this->PlaceOutline(Width, Height);
			this->BuildMesh();
		}

	private:

		void BuildCornerOffsets(
			float Width, float Height
		)
		{
			bool bHasValidSegments =
				this->CornerSegments >= 0;

			check(bHasValidSegments);

			float CornerRadius =
				this->GetCornerRadius(Width, Height);

			this->CornerVertexCount =
				this->CornerSegments + 1;

			float AngleStep =
				UE_HALF_PI / this->CornerVertexCount;

			float HalfStep = AngleStep * 0.5f;
			float HalfStepCosine = FMath::Cos(HalfStep);

			float VertexDistance =
				CornerRadius / HalfStepCosine;

			this->CornerOffsets.Reset();

			int32 CornerCount = this->CornerSigns.Num();

			for (
				int32 CornerIndex=0;
				CornerIndex<CornerCount;
				CornerIndex++
			)
			{
				const FVector& CornerSign =
					this->CornerSigns[CornerIndex];

				FVector ArcCenter =
					-CornerSign * CornerRadius;

				float CornerAngle =
					UE_HALF_PI * CornerIndex;

				float FirstAngle =
					UE_PI + CornerAngle + HalfStep;

				this->CreateCornerOffsets(
					AngleStep, FirstAngle,
					VertexDistance, ArcCenter
				);
			}
		}

		float GetCornerRadius(
			float Width, float Height
		) const
		{
			bool bHasValidRoundness =
				FMath::
					IsWithinInclusive(
						this->Roundness, 0.f, 1.f
					);

			check(bHasValidRoundness);

			float SmallestSide =
				FMath::Min(Width, Height);

			float SmallestHalfSide = SmallestSide * 0.5f;

			return this->Roundness * SmallestHalfSide;
		}

		void CreateCornerOffsets(
			float AngleStep, float FirstAngle,
			float VertexDistance,
			const FVector& ArcCenter
		)
		{
			for (
				int32 ArcIndex=0;
				ArcIndex<this->CornerVertexCount;
				ArcIndex++
			)
			{
				float Angle =
					FirstAngle + AngleStep * ArcIndex;

				float ScreenUp = FMath::Sin(Angle);
				float ScreenRight = FMath::Cos(Angle);

				FVector Direction =
					FVector(ScreenUp, ScreenRight, 0.f);

				FVector Offset =
					ArcCenter + Direction * VertexDistance;

				this->CornerOffsets.Add(Offset);
			}
		}

		void BuildVertices(
			float Width, float Height
		)
		{
			int32 OutlineCount = this->CornerOffsets.Num();
			int32 VertexCount = OutlineCount + 1;

			FVector Center = FVector::ZeroVector;

			this->Vertices.Init(Center, VertexCount);

			this->PlaceOutline(Width, Height);
		}

		void PlaceOutline(
			float Width, float Height
		)
		{
			float HalfWidth = Width * 0.5f;
			float HalfHeight = Height * 0.5f;

			FVector HalfSize =
				FVector(HalfHeight, HalfWidth, 0.f);

			int32 CornerCount = this->CornerSigns.Num();

			for (
				int32 CornerIndex=0;
				CornerIndex<CornerCount;
				CornerIndex++
			)
			{
				const FVector& CornerSign =
					this->CornerSigns[CornerIndex];

				FVector Corner = CornerSign * HalfSize;

				int32 FirstOutlineIndex =
					CornerIndex * this->CornerVertexCount;

				this->PlaceOutlineVertices(
					FirstOutlineIndex, Corner
				);
			}
		}

		void PlaceOutlineVertices(
			int32 FirstOutlineIndex,
			const FVector& Corner
		)
		{
			for (
				int32 ArcIndex=0;
				ArcIndex<this->CornerVertexCount;
				ArcIndex++
			)
			{
				int32 OutlineIndex =
					FirstOutlineIndex + ArcIndex;

				int32 VertexIndex = OutlineIndex + 1;

				const FVector& Offset =
					this->CornerOffsets[OutlineIndex];

				this->Vertices[VertexIndex] =
					Corner + Offset;
			}
		}

		void BuildMesh()
		{
			TArray<int32> Triangles = this->GetTriangles();

			int32 VertexCount = this->Vertices.Num();

			FVector Up = FVector::UpVector;

			TArray<FVector> Normals;
			Normals.Init(Up, VertexCount);

			TArray<FVector2D> UVs;
			TArray<FLinearColor> VertexColors;
			TArray<FProcMeshTangent> Tangents;

			this->CreateMeshSection_LinearColor(
				0,
				this->Vertices,
				Triangles,
				Normals,
				UVs,
				VertexColors,
				Tangents,
				true
			);
		}

		TArray<int32> GetTriangles() const
		{
			int32 OutlineCount = this->CornerOffsets.Num();

			int32 CenterIndex = 0;

			TArray<int32> Triangles;

			for (
				int32 OutlineIndex=0;
				OutlineIndex<OutlineCount;
				OutlineIndex++
			)
			{
				int32 VertexIndex =
					OutlineIndex + 1;

				int32 NextOutlineIndex =
					VertexIndex % OutlineCount;

				int32 NextVertexIndex =
					NextOutlineIndex + 1;

				Triangles.Add(CenterIndex);
				Triangles.Add(VertexIndex);
				Triangles.Add(NextVertexIndex);
			}

			return Triangles;
		}

		void ApplyMaterial()
		{
			UMaterialInstanceDynamic* DynamicMaterial =
				UMaterialInstanceDynamic::
				Create(this->BaseMaterial, this);

			FName ColorParameterName =
				TEXT("Color");

			DynamicMaterial->
				SetVectorParameterValue(
					ColorParameterName,
					this->Color
				);

			this->SetMaterial(0, DynamicMaterial);
		}

		TArray<FVector> CornerSigns;

		int32 CornerVertexCount = 0;

		TArray<FVector> CornerOffsets;

		TArray<FVector> Vertices;

		UPROPERTY()
		TObjectPtr<UMaterialInterface> BaseMaterial;

		UPROPERTY(EditAnywhere, Category = "Background")
		FLinearColor Color = FLinearColor::Blue;

		UPROPERTY(
			EditAnywhere,
			Category = "Background",
			meta = (ClampMin = "0")
		)
		int32 CornerSegments = 4;

		float Roundness = 1.0f;
};