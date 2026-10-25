#pragma once

#include "CoreMinimal.h"
#include "FarmSimCharacter.h"
#include "FarmerCharacter.generated.h"

class UFarmingComponent;
class UInputAction;
class UInputMappingContext;

/**
 * The template's third person character plus farming.
 * Reparent BP_ThirdPersonCharacter to this class so it keeps its mesh, animation and movement input.
 */
UCLASS()
class AFarmerCharacter : public AFarmSimCharacter
{
	GENERATED_BODY()

public:
	AFarmerCharacter();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void SelectHoe();
	void SelectSeeds();
	void SelectWateringCan();
	void SelectHand();

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UFarmingComponent> Farming;

	// Input is created in code so no input assets are needed yet. These can move to editor assets later.
	UPROPERTY()
	TObjectPtr<UInputMappingContext> FarmingMappingContext;

	UPROPERTY()
	TArray<TObjectPtr<UInputAction>> FarmingActions;
};
