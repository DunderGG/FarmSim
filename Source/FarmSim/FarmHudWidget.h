#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class AFarmHUD;

/**
 * The farming HUD's layout, built in Slate so it needs no widget assets.
 * Asks its AFarmHUD for everything it shows, every frame.
 */
class SFarmHudWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFarmHudWidget) {}
		SLATE_ARGUMENT(TWeakObjectPtr<const AFarmHUD>, Hud)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FText GetStatusText() const;
	FText GetInventoryText() const;
	FText GetMessageText() const;
	EVisibility GetMessageVisibility() const;

	TWeakObjectPtr<const AFarmHUD> Hud;
};
