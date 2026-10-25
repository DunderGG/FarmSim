#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FarmingComponent.generated.h"

class AFarmDayCycle;
class AFarmGrid;
class AFarmTile;
class SFarmHud;

UENUM(BlueprintType)
enum class EFarmTool : uint8
{
	Hoe,
	Seeds,
	WateringCan UMETA(DisplayName = "Watering Can"),
	Hand
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFarmToolChanged, EFarmTool);

/**
 * Gives its owner farming tools, a tiny inventory and the ability to sleep.
 * Uses the tile directly in front of the owner, and shows the farming HUD while a local player controls the owner.
 * Saves the farm every time a new day starts and loads it again when play begins.
 */
UCLASS(ClassGroup = (Farming), meta = (BlueprintSpawnableComponent))
class UFarmingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFarmingComponent();

	void SelectTool(EFarmTool Tool);
	EFarmTool GetCurrentTool() const { return CurrentTool; }
	void UseTool();
	void Sleep();

	/** Deletes the save and reloads the level, starting the farm from day 1. */
	void StartNewGame();

	/** Broadcast whenever the current tool changes, including when a save is loaded. */
	FOnFarmToolChanged OnToolChanged;

	int32 GetCurrentDay() const;

	/** Hours since midnight at the start of the current day, past 24 after midnight. */
	float GetTimeOfDay() const;

	/** The latest feedback message, shown on the HUD for MessageDuration seconds. */
	const FString& GetMessage() const { return Message; }
	bool HasMessage() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** How far in front of the owner the targeted tile is, in cm. */
	UPROPERTY(EditAnywhere, Category = "Farming")
	float ReachDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Farming")
	int32 Seeds = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 HarvestedCrops = 0;

	/** How long feedback messages stay on screen, in seconds. */
	UPROPERTY(EditAnywhere, Category = "Farming|HUD", meta = (ClampMin = 0.5))
	float MessageDuration = 3.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	AFarmTile* GetTargetTile() const;
	void StartNextDay(const TCHAR* Reason);
	void SaveFarm();
	void LoadFarm();
	void ShowMessage(const FString& NewMessage);
	void AddHudIfLocalPlayer();
	void RemoveHud();

	UPROPERTY()
	TObjectPtr<AFarmGrid> Grid;

	UPROPERTY()
	TObjectPtr<AFarmDayCycle> DayCycle;

	EFarmTool CurrentTool = EFarmTool::Hoe;

	FString Message;
	double MessageEndTime = 0.0;

	TSharedPtr<SFarmHud> Hud;
};
