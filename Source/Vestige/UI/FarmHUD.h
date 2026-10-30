// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FarmHUD.generated.h"

class AFarmDayCycle;
class AFarmerCharacter;
class APawn;
class SFarmHudWidget;
class UFarmingComponent;
class UFarmToolData;

/**
 * Shows the farming HUD for its player: the day and time from the world's AFarmDayCycle, the tool and inventory from
 * the player's UFarmingComponent, the farmer's key hints and the feedback messages UFarmFeedbackSubsystem carries.
 * It only displays. It builds each text when an event says what it shows has changed, so the widget's per-frame
 * bindings only read finished text; the messages are the one thing it keeps, and only until they fade.
 */
UCLASS()
class AFarmHUD : public AHUD
{
	GENERATED_BODY()

public:
	/**
	 * "Day 3   18:40   |   Hoe". Read every frame for the clock, but only rebuilt when the day, the shown time or
	 * the tool changes.
	 */
	FText GetStatusText() const;

	/** "Seeds: 9   Harvested: 2". Rebuilt when the farming component's inventory changes. */
	const FText& GetInventoryText() const { return InventoryText; }

	/** The key hints, built from the farmer's input mappings. Rebuilt when the toolbar or the pawn changes. */
	const FText& GetControlsText() const { return ControlsText; }

	/** The feedback messages showing now, oldest first, one per line. Rebuilt only when a message arrives or fades. */
	const FText& GetMessagesText() const { return MessagesText; }
	bool HasMessages() const { return !Messages.IsEmpty(); }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	struct FMessage
	{
		FText Text;

		/** World time, in seconds, when the message fades. */
		double EndTime = 0.0;
	};

	/** Follows the new pawn's farming component and key hints. Bound to the controller's OnPossessedPawnChanged. */
	UFUNCTION()
	void FollowPawn(APawn* OldPawn, APawn* NewPawn);

	void StopFollowingFarming();

	/** Rebuilds the tool's name and the key hints, which name the toolbar's tools. Bound to OnToolChanged. */
	void RefreshToolTexts(const UFarmToolData* Tool);

	/** Bound to OnInventoryChanged. */
	void RefreshInventoryText();

	void RefreshControlsText();

	/** Shows Message under the ones already showing for MessageDuration seconds. Bound to UFarmFeedbackSubsystem. */
	void ShowMessage(const FText& Message);

	/** Drops the messages whose time is up, and waits for the next one to fade. */
	void RemoveFadedMessages();

	void RebuildMessagesText();

	/** How long feedback messages stay on screen, in seconds. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|HUD", meta = (AllowPrivateAccess = "true", ClampMin = 0.5))
	float MessageDuration = 3.f;

	/** Most messages on screen at once. A new message beyond this pushes out the oldest. */
	UPROPERTY(EditDefaultsOnly, Category = "Farming|HUD", meta = (AllowPrivateAccess = "true", ClampMin = 1))
	int32 MaxMessages = 3;

	TWeakObjectPtr<const AFarmDayCycle> DayCycle;
	TWeakObjectPtr<UFarmingComponent> Farming;
	TWeakObjectPtr<const AFarmerCharacter> Farmer;

	FText ToolText;
	FText InventoryText;
	FText ControlsText;

	// The status line as last built, and the day and minute it shows. GetStatusText is a const per-frame getter that
	// rebuilds them only when they change.
	mutable FText StatusText;
	mutable int32 StatusDay = INDEX_NONE;
	mutable int32 StatusClockMinutes = INDEX_NONE;

	TArray<FMessage> Messages;
	FText MessagesText;
	FTimerHandle FadeTimer;

	TSharedPtr<SFarmHudWidget> HudWidget;
};
