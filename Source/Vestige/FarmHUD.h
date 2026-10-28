// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FarmHUD.generated.h"

class AFarmDayCycle;
class APawn;
class SFarmHudWidget;
class UFarmingComponent;

/**
 * Shows the farming HUD for its player: the day and time from the level's AFarmDayCycle,
 * the tool and inventory from the player's UFarmingComponent, and short feedback messages.
 * It only displays; the messages are the one thing it keeps, and only until they fade.
 */
UCLASS()
class AFarmHUD : public AHUD
{
	GENERATED_BODY()

public:
	/** The HUD of the player controlling Pawn, or nullptr if it has none or it isn't a farm HUD. */
	static AFarmHUD* GetFor(const APawn* Pawn);

	/** Shows a feedback message for MessageDuration seconds, replacing the current one. */
	void ShowMessage(const FString& NewMessage);

	/** Adds to the current message and restarts its timer, or shows Extra on its own if none is showing. */
	void AppendMessage(const FString& Extra);

	const FString& GetMessage() const { return Message; }
	bool HasMessage() const;

	const UFarmingComponent* GetFarming() const;
	const AFarmDayCycle* GetDayCycle() const;

	/** How long feedback messages stay on screen, in seconds. */
	UPROPERTY(EditAnywhere, Category = "Farming|HUD", meta = (ClampMin = 0.5))
	float MessageDuration = 3.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FString Message;
	double MessageEndTime = 0.0;

	TSharedPtr<SFarmHudWidget> HudWidget;
};
