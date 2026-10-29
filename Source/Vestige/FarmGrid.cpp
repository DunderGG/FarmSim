// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "FarmGrid.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "FarmActorLookup.h"
#include "FarmDayCycle.h"
#include "FarmTile.h"
#include "Vestige.h"

namespace FarmGridEditor
{
	/** Half the height of the box that outlines the grid in the editor, in cm. Only needs to be visible. */
	constexpr float BoundsHalfHeight = 10.f;
}

AFarmGrid::AFarmGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Bounds->SetupAttachment(RootComponent);
	Bounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Bounds->SetHiddenInGame(true);

	TileClass = AFarmTile::StaticClass();
}

void AFarmGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FVector HalfSize(Width * TileSize * 0.5f, Height * TileSize * 0.5f, FarmGridEditor::BoundsHalfHeight);
	Bounds->SetBoxExtent(HalfSize);
	Bounds->SetRelativeLocation(FVector(HalfSize.X, HalfSize.Y, 0.f));
}

void AFarmGrid::BeginPlay()
{
	Super::BeginPlay();

	DayCycle = AFarmDayCycle::Get(GetWorld());
	if (AFarmDayCycle* Cycle = DayCycle.Get())
	{
		Cycle->OnNewDay.AddUObject(this, &AFarmGrid::AdvanceTiles);
	}

	SpawnTiles();
	bTilesReady = true;
	OnTilesReady.Broadcast();
}

void AFarmGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AFarmDayCycle* Cycle = DayCycle.Get())
	{
		Cycle->OnNewDay.RemoveAll(this);
	}

	// When the grid alone goes (destroyed, or its sublevel streamed out), its tiles go with it. When the whole world
	// ends, the world destroys them itself.
	if (EndPlayReason == EEndPlayReason::Destroyed || EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		for (AFarmTile* Tile : Tiles)
		{
			if (IsValid(Tile))
			{
				Tile->Destroy();
			}
		}
	}
	Tiles.Reset();
	bTilesReady = false;
	Super::EndPlay(EndPlayReason);
}

AFarmGrid* AFarmGrid::Find(const UWorld* World)
{
	return Cast<AFarmGrid>(FarmActorLookup::FindSingle(World, StaticClass()));
}

void AFarmGrid::SpawnTiles()
{
	if (!TileClass)
	{
		UE_LOG(LogVestige, Warning, TEXT("%s has no Tile Class, so the field is empty."), *GetName());
		return;
	}

	const FVector TileScale(TileSize / AFarmTile::UnscaledSize);
	const FTransform GridTransform = GetActorTransform();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Tiles.Reserve(Width * Height);
	for (int32 Y = 0; Y < Height; ++Y)
	{
		for (int32 X = 0; X < Width; ++X)
		{
			const FVector LocalCenter((X + 0.5f) * TileSize, (Y + 0.5f) * TileSize, 0.f);
			const FTransform TileTransform(GetActorRotation(), GridTransform.TransformPosition(LocalCenter), TileScale);
			Tiles.Add(GetWorld()->SpawnActor<AFarmTile>(TileClass, TileTransform, SpawnParams));
		}
	}
}

AFarmTile* AFarmGrid::GetTileAtLocation(const FVector& WorldLocation) const
{
	const FVector Local = GetActorTransform().InverseTransformPosition(WorldLocation);
	const int32 X = FMath::FloorToInt(Local.X / TileSize);
	const int32 Y = FMath::FloorToInt(Local.Y / TileSize);
	if (X < 0 || X >= Width || Y < 0 || Y >= Height)
	{
		return nullptr;
	}

	const int32 Index = Y * Width + X;
	return Tiles.IsValidIndex(Index) ? Tiles[Index].Get() : nullptr;
}

void AFarmGrid::AdvanceTiles(int32 NewDay)
{
	for (AFarmTile* Tile : Tiles)
	{
		if (Tile)
		{
			Tile->AdvanceDay();
		}
	}
}
