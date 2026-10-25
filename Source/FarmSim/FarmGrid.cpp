#include "FarmGrid.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "FarmDayCycle.h"
#include "FarmTile.h"

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

	const FVector HalfSize(Width * TileSize * 0.5f, Height * TileSize * 0.5f, 10.f);
	Bounds->SetBoxExtent(HalfSize);
	Bounds->SetRelativeLocation(FVector(HalfSize.X, HalfSize.Y, 0.f));
}

void AFarmGrid::BeginPlay()
{
	Super::BeginPlay();

	if (AFarmDayCycle* DayCycle = AFarmDayCycle::Get(GetWorld()))
	{
		DayCycle->OnNewDay.AddUObject(this, &AFarmGrid::AdvanceTiles);
	}

	if (!TileClass)
	{
		return;
	}

	// Engine shapes are 100 units wide, so scale tiles to match TileSize.
	const FVector TileScale(TileSize / 100.f);
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
