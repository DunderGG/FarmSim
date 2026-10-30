// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "Template/VestigeCharacter.h"
#include "FarmerCharacter.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UFarmingComponent;
class UFarmRestComponent;
class UFarmTargetHighlightComponent;
class UFarmToolData;
class UInputAction;
class UInputMappingContext;
class UStaticMeshComponent;

/** A key the player can press and what it does, for on-screen hints. */
struct FFarmKeyHint
{
	FText Key;
	FText Action;
};

/**
 * The template's third person character plus farming.
 * Reparent BP_ThirdPersonCharacter to this class so it keeps its mesh, animation and movement input.
 * Owns the farming key bindings, and describes them for the HUD's key hints.
 */
UCLASS()
class AFarmerCharacter : public AVestigeCharacter
{
	GENERATED_BODY()

public:
	AFarmerCharacter();

	/** The toolbar keys in slot order, each named after the tool in its slot. Empty slots are left out. */
	TArray<FFarmKeyHint> GetToolbarKeyHints() const;

	/**
	 * The other farming keys and what they do (use tool, sleep, new game), in the order they are mapped.
	 * Read from the mapping context, so the hints always name the keys that are really bound.
	 */
	TArray<FFarmKeyHint> GetActionKeyHints() const;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void UnPossessed() override;

private:
	void StartNewGame();

	/** Creates the farming input actions and maps their keys. Runs once, before the farmer is first possessed. */
	void CreateInput();

	/** A new input action mapped to Key in the farming mapping context. A Description makes it show in the key hints. */
	UInputAction* AddInputAction(const FKey& Key, const FText& Description);

	/** The key Action is mapped to in the farming mapping context, or an invalid key. */
	FKey GetMappedKey(const UInputAction* Action) const;

	UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem() const;

	/** Shows the tool's held mesh in the hand, or nothing for an empty slot or a tool without one. */
	void ShowHeldTool(const UFarmToolData* Tool);

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UFarmingComponent> Farming;

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UFarmRestComponent> Rest;

	/** Frames the tile the farming component targets. */
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UFarmTargetHighlightComponent> TargetHighlight;

	/** Attached to the right hand. Move and rotate it in the Blueprint to line up each tool's grip. */
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<UStaticMeshComponent> HeldTool;

	// Input is created in code so no input assets are needed yet. These can move to IA_ and IMC_ assets later.
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> FarmingMappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> UseToolAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> SleepAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> NewGameAction;

	/** One action per number key; the index is the toolbar slot it selects. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UInputAction>> ToolbarSlotActions;
};
