// Copyright (C) 2026 David Bennehag and the FarmSim contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-FarmSim-Unreal-Exception

#include "FarmTile.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace FarmTileColors
{
	// Placeholder colors until the Blender assets replace these shapes.
	const FLinearColor Grass(0.15f, 0.45f, 0.10f);
	const FLinearColor Tilled(0.35f, 0.20f, 0.08f);
	const FLinearColor Watered(0.12f, 0.07f, 0.03f);
	const FLinearColor Growing(0.20f, 0.70f, 0.15f);
	const FLinearColor Ripe(1.00f, 0.60f, 0.05f);
}

namespace FarmTilePlaceholder
{
	// The engine cube is 100 units wide, so this makes a thin 95x95 cm slab with a small gap between tiles.
	const FVector SoilScale(0.95f, 0.95f, 0.04f);
	const FVector SoilLocation(0.f, 0.f, 2.f);
	const float SoilTop = 4.f;
}

AFarmTile::AFarmTile()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	PlaceholderSoilMesh = CubeMesh.Object;
	PlaceholderCropMesh = CylinderMesh.Object;

	// Both meshes are swapped and moved during play, which static components aren't allowed to do.
	SoilMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoilMesh"));
	SoilMesh->SetupAttachment(RootComponent);
	SoilMesh->SetMobility(EComponentMobility::Movable);
	SoilMesh->SetStaticMesh(PlaceholderSoilMesh);
	SoilMesh->SetMaterial(0, ShapeMaterial.Object);
	SoilMesh->SetRelativeScale3D(FarmTilePlaceholder::SoilScale);
	SoilMesh->SetRelativeLocation(FarmTilePlaceholder::SoilLocation);
	SoilMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CropMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CropMesh"));
	CropMesh->SetupAttachment(RootComponent);
	CropMesh->SetMobility(EComponentMobility::Movable);
	CropMesh->SetStaticMesh(PlaceholderCropMesh);
	CropMesh->SetMaterial(0, ShapeMaterial.Object);
	CropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CropMesh->SetVisibility(false);
}

void AFarmTile::BeginPlay()
{
	Super::BeginPlay();

	SoilMaterial = SoilMesh->CreateDynamicMaterialInstance(0);
	CropMaterial = CropMesh->CreateDynamicMaterialInstance(0);
	RefreshVisuals();
}

bool AFarmTile::Till()
{
	if (State != EFarmTileState::Grass)
	{
		return false;
	}
	State = EFarmTileState::Tilled;
	RefreshVisuals();
	return true;
}

bool AFarmTile::Plant()
{
	if (State != EFarmTileState::Tilled)
	{
		return false;
	}
	State = EFarmTileState::Planted;
	GrowthDays = 0;
	RefreshVisuals();
	return true;
}

bool AFarmTile::Water()
{
	if (State == EFarmTileState::Grass || bWatered)
	{
		return false;
	}
	bWatered = true;
	RefreshVisuals();
	return true;
}

bool AFarmTile::Harvest()
{
	if (!IsRipe())
	{
		return false;
	}
	State = EFarmTileState::Tilled;
	GrowthDays = 0;
	RefreshVisuals();
	return true;
}

void AFarmTile::AdvanceDay()
{
	if (State == EFarmTileState::Planted && bWatered && !IsRipe())
	{
		++GrowthDays;
	}
	bWatered = false;
	RefreshVisuals();
}

void AFarmTile::RestoreState(EFarmTileState SavedState, bool bSavedWatered, int32 SavedGrowthDays)
{
	State = SavedState;
	bWatered = bSavedWatered && State != EFarmTileState::Grass;
	GrowthDays = State == EFarmTileState::Planted ? FMath::Clamp(SavedGrowthDays, 0, DaysToMature) : 0;
	RefreshVisuals();
}

void AFarmTile::RefreshVisuals()
{
	RefreshSoil();
	RefreshCrop();
}

void AFarmTile::RefreshSoil()
{
	if (State != EFarmTileState::Grass && TilledSoilMesh)
	{
		SoilMesh->SetStaticMesh(TilledSoilMesh);
		SoilMesh->SetRelativeScale3D(FVector::OneVector);
		SoilMesh->SetRelativeLocation(FVector::ZeroVector);

		// Use the mesh's own materials, or the watered material on every slot.
		SoilMesh->EmptyOverrideMaterials();
		if (bWatered && WateredSoilMaterial)
		{
			for (int32 Index = 0; Index < SoilMesh->GetNumMaterials(); ++Index)
			{
				SoilMesh->SetMaterial(Index, WateredSoilMaterial);
			}
		}
		return;
	}

	SoilMesh->SetStaticMesh(PlaceholderSoilMesh);
	SoilMesh->SetRelativeScale3D(FarmTilePlaceholder::SoilScale);
	SoilMesh->SetRelativeLocation(FarmTilePlaceholder::SoilLocation);
	if (SoilMaterial)
	{
		const FLinearColor SoilColor = State == EFarmTileState::Grass ? FarmTileColors::Grass
			: bWatered ? FarmTileColors::Watered
			: FarmTileColors::Tilled;
		SoilMaterial->SetVectorParameterValue(TEXT("Color"), SoilColor);
		SoilMesh->SetMaterial(0, SoilMaterial);
	}
}

void AFarmTile::RefreshCrop()
{
	const bool bHasCrop = State == EFarmTileState::Planted;
	CropMesh->SetVisibility(bHasCrop);
	if (!bHasCrop)
	{
		return;
	}

	const float SurfaceZ = TilledSoilMesh ? SoilSurfaceHeight : FarmTilePlaceholder::SoilTop;
	UStaticMesh* StageMesh = CropStageMeshes.IsEmpty() ? nullptr
		: CropStageMeshes[FMath::Min(GrowthDays, CropStageMeshes.Num() - 1)].Get();
	if (StageMesh)
	{
		CropMesh->SetStaticMesh(StageMesh);
		CropMesh->EmptyOverrideMaterials();
		CropMesh->SetRelativeScale3D(FVector::OneVector);
		CropMesh->SetRelativeLocation(FVector(0.f, 0.f, SurfaceZ));
		return;
	}

	// The engine cylinder is 100 units tall, centered on its origin. Grow it taller each stage.
	const float Height = 0.15f + 0.25f * GrowthDays;
	CropMesh->SetStaticMesh(PlaceholderCropMesh);
	CropMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, Height));
	CropMesh->SetRelativeLocation(FVector(0.f, 0.f, SurfaceZ + Height * 50.f));
	if (CropMaterial)
	{
		CropMaterial->SetVectorParameterValue(TEXT("Color"), IsRipe() ? FarmTileColors::Ripe : FarmTileColors::Growing);
		CropMesh->SetMaterial(0, CropMaterial);
	}
}
