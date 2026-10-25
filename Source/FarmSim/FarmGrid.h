#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmGrid.generated.h"

class AFarmTile;
class ADirectionalLight;
class ASkyLight;
class UBoxComponent;

/**
 * A rectangle of farm tiles, spawned at BeginPlay with the actor's location as one corner.
 * Also keeps the day counter and the clock for now, and moves the sun to match the time.
 */
UCLASS()
class AFarmGrid : public AActor
{
	GENERATED_BODY()

public:
	AFarmGrid();

	/** Returns the tile under a world location, ignoring height, or nullptr if it's outside the grid. */
	AFarmTile* GetTileAtLocation(const FVector& WorldLocation) const;

	/** Ends the current day, advances every tile and sets the clock to the next morning. */
	void AdvanceDay();

	int32 GetCurrentDay() const { return CurrentDay; }

	/** Hours since midnight at the start of the current day. Goes past 24 after midnight, up to DayEndHour. */
	float GetTimeOfDay() const { return TimeOfDay; }

	/** True once the clock reaches DayEndHour and the player should pass out. */
	bool IsPastBedtime() const { return TimeOfDay >= DayEndHour; }

	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 1))
	int32 Width = 8;

	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 1))
	int32 Height = 8;

	/** Size of one tile in Unreal units (cm). */
	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 10))
	float TileSize = 100.f;

	/** Tile actor to spawn. Swap in a Blueprint subclass later to change how tiles look. */
	UPROPERTY(EditAnywhere, Category = "Farming")
	TSubclassOf<AFarmTile> TileClass;

	/** Real seconds per in-game hour. */
	UPROPERTY(EditAnywhere, Category = "Farming|Time", meta = (ClampMin = 0.1))
	float RealSecondsPerGameHour = 40.f;

	/** Hour each day starts at after sleeping. */
	UPROPERTY(EditAnywhere, Category = "Farming|Time", meta = (ClampMin = 0, ClampMax = 23))
	float DayStartHour = 6.f;

	/** Hour the player passes out. 26 is 2:00 the next morning. */
	UPROPERTY(EditAnywhere, Category = "Farming|Time", meta = (ClampMin = 1, ClampMax = 30))
	float DayEndHour = 26.f;

	UPROPERTY(EditAnywhere, Category = "Farming|Time", meta = (ClampMin = 0, ClampMax = 23))
	float SunriseHour = 6.f;

	UPROPERTY(EditAnywhere, Category = "Farming|Time", meta = (ClampMin = 1, ClampMax = 24))
	float SunsetHour = 20.f;

	/** Light moved with the time of day. Left empty, the first DirectionalLight in the level is used. */
	UPROPERTY(EditAnywhere, Category = "Farming|Lighting")
	TObjectPtr<ADirectionalLight> Sun;

	/** Dimmed at night. Left empty, the first SkyLight in the level is used. */
	UPROPERTY(EditAnywhere, Category = "Farming|Lighting")
	TObjectPtr<ASkyLight> SkyLight;

	/** Highest angle of the sun above the horizon at midday, in degrees. */
	UPROPERTY(EditAnywhere, Category = "Farming|Lighting", meta = (ClampMin = 10, ClampMax = 90))
	float MaxSunElevation = 65.f;

	/** Share of the daytime light intensity left at night, for moonlight. */
	UPROPERTY(EditAnywhere, Category = "Farming|Lighting", meta = (ClampMin = 0, ClampMax = 1))
	float NightLightFraction = 0.05f;

	UPROPERTY(EditAnywhere, Category = "Farming|Lighting")
	FLinearColor SunriseColor = FLinearColor(1.f, 0.55f, 0.3f);

	UPROPERTY(EditAnywhere, Category = "Farming|Lighting")
	FLinearColor MoonColor = FLinearColor(0.5f, 0.6f, 1.f);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	void FindLights();
	void UpdateLighting();

	/** Outlines the grid in the editor so it's easy to place. Hidden in game. */
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UBoxComponent> Bounds;

	UPROPERTY()
	TArray<TObjectPtr<AFarmTile>> Tiles;

	int32 CurrentDay = 1;
	float TimeOfDay = 6.f;

	// The lights' settings as placed in the level, used as the daytime values.
	float SunYaw = 0.f;
	float DaySunIntensity = 0.f;
	FLinearColor DaySunColor = FLinearColor::White;
	float DaySkyIntensity = 0.f;
};
