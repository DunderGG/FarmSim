#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UFarmingComponent;

/**
 * The farming HUD, built in Slate so it needs no widget assets.
 * Reads everything from the farming component every frame.
 */
class SFarmHud : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFarmHud) {}
		SLATE_ARGUMENT(TWeakObjectPtr<UFarmingComponent>, Farming)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FText GetStatusText() const;
	FText GetInventoryText() const;
	FText GetMessageText() const;
	EVisibility GetMessageVisibility() const;

	TWeakObjectPtr<UFarmingComponent> Farming;
};
