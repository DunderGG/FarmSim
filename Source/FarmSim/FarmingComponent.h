#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FarmingComponent.generated.h"

class AFarmGrid;
class AFarmTile;

UENUM(BlueprintType)
enum class EFarmTool : uint8
{
	Hoe,
	Seeds,
	WateringCan UMETA(DisplayName = "Watering Can"),
	Hand
};

/**
 * Gives its owner farming tools, a tiny inventory and the ability to sleep.
 * Uses the tile directly in front of the owner. Shows a debug HUD until step 4 adds a real one.
 */
UCLASS(ClassGroup = (Farming), meta = (BlueprintSpawnableComponent))
class UFarmingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFarmingComponent();

	void SelectTool(EFarmTool Tool);
	void UseTool();
	void Sleep();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** How far in front of the owner the targeted tile is, in cm. */
	UPROPERTY(EditAnywhere, Category = "Farming")
	float ReachDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Farming")
	int32 Seeds = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 HarvestedCrops = 0;

protected:
	virtual void BeginPlay() override;

private:
	AFarmTile* GetTargetTile() const;
	void ShowMessage(const FString& Message) const;

	UPROPERTY()
	TObjectPtr<AFarmGrid> Grid;

	EFarmTool CurrentTool = EFarmTool::Hoe;
};
