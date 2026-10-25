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

AFarmTile::AFarmTile()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	// The engine cube is 100 units wide, so this makes a thin 95x95 cm slab with a small gap between tiles.
	SoilMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SoilMesh"));
	SoilMesh->SetupAttachment(RootComponent);
	SoilMesh->SetStaticMesh(CubeMesh.Object);
	SoilMesh->SetMaterial(0, ShapeMaterial.Object);
	SoilMesh->SetRelativeScale3D(FVector(0.95f, 0.95f, 0.04f));
	SoilMesh->SetRelativeLocation(FVector(0.f, 0.f, 2.f));
	SoilMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CropMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CropMesh"));
	CropMesh->SetupAttachment(RootComponent);
	CropMesh->SetStaticMesh(CylinderMesh.Object);
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

void AFarmTile::RefreshVisuals()
{
	if (SoilMaterial)
	{
		const FLinearColor SoilColor = State == EFarmTileState::Grass ? FarmTileColors::Grass
			: bWatered ? FarmTileColors::Watered
			: FarmTileColors::Tilled;
		SoilMaterial->SetVectorParameterValue(TEXT("Color"), SoilColor);
	}

	const bool bHasCrop = State == EFarmTileState::Planted;
	CropMesh->SetVisibility(bHasCrop);
	if (!bHasCrop)
	{
		return;
	}

	// The engine cylinder is 100 units tall, centered on its origin. Grow it taller each stage.
	const float Height = 0.15f + 0.25f * GrowthDays;
	CropMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, Height));
	CropMesh->SetRelativeLocation(FVector(0.f, 0.f, 4.f + Height * 50.f));
	if (CropMaterial)
	{
		CropMaterial->SetVectorParameterValue(TEXT("Color"), IsRipe() ? FarmTileColors::Ripe : FarmTileColors::Growing);
	}
}
