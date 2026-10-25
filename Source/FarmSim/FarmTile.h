#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FarmTile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class EFarmTileState : uint8
{
	Grass,
	Tilled,
	Planted
};

/** One square of farmland: can be tilled, planted, watered and harvested. */
UCLASS()
class AFarmTile : public AActor
{
	GENERATED_BODY()

public:
	AFarmTile();

	// Each action returns true if it changed the tile.
	bool Till();
	bool Plant();
	bool Water();
	bool Harvest();

	/** Called once per in-game day. Watered crops grow one stage, then the soil dries out. */
	void AdvanceDay();

	EFarmTileState GetState() const { return State; }
	bool IsWatered() const { return bWatered; }
	bool IsRipe() const { return State == EFarmTileState::Planted && GrowthDays >= DaysToMature; }

	/** Days of watering a crop needs before it can be harvested. */
	UPROPERTY(EditAnywhere, Category = "Farming", meta = (ClampMin = 1))
	int32 DaysToMature = 3;

protected:
	virtual void BeginPlay() override;

private:
	void RefreshVisuals();

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UStaticMeshComponent> SoilMesh;

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UStaticMeshComponent> CropMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> SoilMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CropMaterial;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	EFarmTileState State = EFarmTileState::Grass;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	bool bWatered = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Farming")
	int32 GrowthDays = 0;
};
