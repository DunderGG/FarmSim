// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#pragma once

#include "CoreMinimal.h"

/**
 * The maths of picking the tile the farmer works on: the neighbouring tile in one of the eight grid directions.
 * Pure functions on grid-space values, so they are tested without a world. UFarmingComponent owns targeting and uses them.
 */
namespace FarmTileTargeting
{
	/**
	 * The grid step (column, row) of the nearest of the eight directions to Direction, which is in the grid's own space.
	 * X and Y are each -1, 0 or 1, and each direction covers 45 degrees. A zero Direction returns (0, 0).
	 */
	FIntPoint SnapToStep(const FVector2D& Direction);

	/** The neighbour of the tile at Coordinates one Step away. A zero Step returns Coordinates. */
	FIntPoint GetNeighbour(const FIntPoint& Coordinates, const FIntPoint& Step);
}
