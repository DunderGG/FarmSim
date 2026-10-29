// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "VestigeCharacter.h"
#include "FarmerCharacter.generated.h"

class UFarmingComponent;
class UFarmToolData;
class UInputAction;
class UInputMappingContext;
class UStaticMeshComponent;

/**
 * The template's third person character plus farming.
 * Reparent BP_ThirdPersonCharacter to this class so it keeps its mesh, animation and movement input.
 */
UCLASS()
class AFarmerCharacter : public AVestigeCharacter
{
	GENERATED_BODY()

public:
	AFarmerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void StartNewGame();

	/** Shows the tool's held mesh in the hand, or nothing for an empty slot or a tool without one. */
	void ShowHeldTool(const UFarmToolData* Tool);

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
