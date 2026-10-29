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
 * Shows the farming HUD for its player: the day and time from the world's AFarmDayCycle,
 * the tool and inventory from the player's UFarmingComponent, and the feedback messages UFarmFeedbackSubsystem carries.
 * It only displays; the messages are the one thing it keeps, and only until they fade.
 */
UCLASS()
class AFarmHUD : public AHUD
{
	GENERATED_BODY()

public:
	/** The feedback messages showing now, oldest first, one per line. Rebuilt only when a message arrives or fades. */
	const FText& GetMessagesText() const { return MessagesText; }
	bool HasMessages() const { return !Messages.IsEmpty(); }

	/** The farming component of the pawn this HUD's player controls, or nullptr. Kept, so it is cheap to call every frame. */
	const UFarmingComponent* GetFarming() const { return Farming.Get(); }

	/** The world's day cycle, or nullptr. Kept, so it is cheap to call every frame. */
	const AFarmDayCycle* GetDayCycle() const { return DayCycle.Get(); }

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

	/** Keeps the new pawn's farming component. Bound to the controller's OnPossessedPawnChanged. */
	UFUNCTION()
	void FollowPawn(APawn* OldPawn, APawn* NewPawn);

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

	TArray<FMessage> Messages;
	FText MessagesText;
	FTimerHandle FadeTimer;

	TSharedPtr<SFarmHudWidget> HudWidget;

	TWeakObjectPtr<const AFarmDayCycle> DayCycle;
	TWeakObjectPtr<const UFarmingComponent> Farming;
};
