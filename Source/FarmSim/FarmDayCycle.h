#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmDayCycle.generated.h"

class ADirectionalLight;
class ASkyLight;
class UPostProcessComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFarmNewDay, int32 /* Day */);

/**
 * Keeps the day counter and the clock, and moves the sun, sky light and exposure to match the time.
 * Place one in the level to tweak its settings. If there is none, one is spawned with default settings.
 */
UCLASS()
class AFarmDayCycle : public AActor
{
	GENERATED_BODY()

public:
	AFarmDayCycle();

	/** Returns the level's day cycle, spawning one if the level has none. */
	static AFarmDayCycle* Get(UWorld* World);

	/** Returns the level's day cycle, or nullptr if there is none yet. Never spawns one. */
	static AFarmDayCycle* Find(const UWorld* World);

	/** Ends the current day and sets the clock to the next morning, then broadcasts OnNewDay. */
	void StartNextDay();

	/** Sets the day and clock from a save without broadcasting OnNewDay. */
	void RestoreClock(int32 SavedDay, float SavedTimeOfDay);

	int32 GetCurrentDay() const { return CurrentDay; }

	/** Hours since midnight at the start of the current day. Goes past 24 after midnight, up to DayEndHour. */
	float GetTimeOfDay() const { return TimeOfDay; }

	/** True once the clock reaches DayEndHour and the player should pass out. */
	bool IsPastBedtime() const { return TimeOfDay >= DayEndHour; }

	/** Broadcast with the new day's number every time a new day starts. The farm reacts here, e.g. crops grow. */
	FOnFarmNewDay OnNewDay;

	/** Broadcast right after OnNewDay, once everything has reacted to the new day. Saving listens here. */
	FOnFarmNewDay OnNewDayFinished;

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
	float NightLightFraction = 0.03f;

	/**
	 * Exposure change at night, in stops. Auto-exposure brightens dark scenes back up,
	 * so this pulls it down again. More negative is darker; 0 turns it off.
	 */
	UPROPERTY(EditAnywhere, Category = "Farming|Lighting", meta = (ClampMin = -8, ClampMax = 0))
	float NightExposureBias = -2.5f;

	UPROPERTY(EditAnywhere, Category = "Farming|Lighting")
	FLinearColor SunriseColor = FLinearColor(1.f, 0.55f, 0.3f);

	UPROPERTY(EditAnywhere, Category = "Farming|Lighting")
	FLinearColor MoonColor = FLinearColor(0.5f, 0.6f, 1.f);

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	void FindLights();
	void UpdateLighting();

	/** Covers the whole level and darkens the exposure at night. */
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UPostProcessComponent> NightPostProcess;

	int32 CurrentDay = 1;
	float TimeOfDay = 6.f;

	// The lights' settings as placed in the level, used as the daytime values.
	float SunYaw = 0.f;
	float DaySunIntensity = 0.f;
	FLinearColor DaySunColor = FLinearColor::White;
	float DaySkyIntensity = 0.f;
};
