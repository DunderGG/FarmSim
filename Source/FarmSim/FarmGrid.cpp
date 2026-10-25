#include "FarmGrid.h"

#include "Components/BoxComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "FarmTile.h"

AFarmGrid::AFarmGrid()
{
	PrimaryActorTick.bCanEverTick = true;

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

	TimeOfDay = DayStartHour;
	FindLights();
	UpdateLighting();

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

void AFarmGrid::AdvanceDay()
{
	++CurrentDay;
	TimeOfDay = DayStartHour;
	UpdateLighting();
	for (AFarmTile* Tile : Tiles)
	{
		if (Tile)
		{
			Tile->AdvanceDay();
		}
	}
}

void AFarmGrid::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// The clock stops at DayEndHour until the player sleeps or passes out.
	TimeOfDay = FMath::Min(TimeOfDay + DeltaSeconds / RealSecondsPerGameHour, DayEndHour);
	UpdateLighting();
}

void AFarmGrid::FindLights()
{
	if (!Sun)
	{
		TActorIterator<ADirectionalLight> It(GetWorld());
		Sun = It ? *It : nullptr;
	}
	if (!SkyLight)
	{
		TActorIterator<ASkyLight> It(GetWorld());
		SkyLight = It ? *It : nullptr;
	}

	if (Sun)
	{
		// Static or stationary lights can't be turned at runtime. Setting the light to Movable in the level avoids this.
		ULightComponent* SunLight = Sun->GetLightComponent();
		if (SunLight->Mobility != EComponentMobility::Movable)
		{
			SunLight->SetMobility(EComponentMobility::Movable);
		}
		SunYaw = Sun->GetActorRotation().Yaw;
		DaySunIntensity = SunLight->Intensity;
		DaySunColor = SunLight->GetLightColor();
	}
	if (SkyLight)
	{
		DaySkyIntensity = SkyLight->GetLightComponent()->Intensity;
	}
}

void AFarmGrid::UpdateLighting()
{
	if (!Sun)
	{
		return;
	}

	// Progress runs 0 to 1 from sunrise to sunset by day, and from sunset to the next sunrise by night.
	// The sun arcs across the sky by day and the moon takes the same path by night.
	const float Hour = FMath::Fmod(TimeOfDay, 24.f);
	const float DayLength = SunsetHour - SunriseHour;
	const bool bDaytime = Hour >= SunriseHour && Hour < SunsetHour;
	const float Progress = bDaytime
		? (Hour - SunriseHour) / DayLength
		: FMath::Fmod(Hour - SunsetHour + 24.f, 24.f) / (24.f - DayLength);
	const float Arc = FMath::Sin(Progress * PI);

	// A few degrees above the horizon at the ends so mornings and evenings get long shadows, not darkness.
	const float Elevation = FMath::Lerp(5.f, MaxSunElevation, Arc);
	Sun->SetActorRotation(FRotator(-Elevation, SunYaw + Progress * 180.f, 0.f));

	// Light fades in after sunrise and out before sunset, warm near the horizon.
	const float Daylight = bDaytime ? FMath::Clamp(Arc * 3.f, 0.f, 1.f) : 0.f;
	const float Brightness = FMath::Lerp(NightLightFraction, 1.f, Daylight);
	ULightComponent* SunLight = Sun->GetLightComponent();
	SunLight->SetIntensity(DaySunIntensity * Brightness);
	SunLight->SetLightColor(bDaytime ? FMath::Lerp(SunriseColor, DaySunColor, FMath::Clamp(Arc * 2.f, 0.f, 1.f)) : MoonColor);

	if (SkyLight)
	{
		SkyLight->GetLightComponent()->SetIntensity(DaySkyIntensity * Brightness);
	}
}
