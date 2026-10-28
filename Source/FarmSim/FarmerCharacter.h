// Copyright (C) 2026 David Bennehag and the FarmSim contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-FarmSim-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "FarmSimCharacter.h"
#include "FarmingComponent.h"
#include "FarmerCharacter.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
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
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Mesh shown in the hand for each tool. Tools without an entry show nothing. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming")
	TMap<EFarmTool, TObjectPtr<UStaticMesh>> ToolMeshes;

private:
	void SelectHoe();
	void SelectSeeds();
	void SelectWateringCan();
	void SelectHand();
	void SelectTool(EFarmTool Tool);
	void StartNewGame();
	void ShowHeldTool(EFarmTool Tool);

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UFarmingComponent> Farming;

	/** Attached to the right hand. Move and rotate it in the Blueprint to line up each tool's grip. */
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UStaticMeshComponent> HeldTool;

	// Input is created in code so no input assets are needed yet. These can move to editor assets later.
	UPROPERTY()
	TObjectPtr<UInputMappingContext> FarmingMappingContext;

	UPROPERTY()
	TArray<TObjectPtr<UInputAction>> FarmingActions;
};
