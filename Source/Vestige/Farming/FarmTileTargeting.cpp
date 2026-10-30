// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#include "Farming/FarmTileTargeting.h"

namespace FarmTileTargeting
{
	/** The angle each of the eight directions covers, in radians: 45 degrees. */
	constexpr double DirectionAngle = UE_DOUBLE_PI / 4.0;

	FIntPoint SnapToStep(const FVector2D& Direction)
	{
		if (Direction.IsNearlyZero())
		{
			return FIntPoint::ZeroValue;
		}

		const int32 Direction8 = FMath::RoundToInt(FMath::Atan2(Direction.Y, Direction.X) / DirectionAngle);
		const double SnappedAngle = Direction8 * DirectionAngle;
		return FIntPoint(FMath::RoundToInt(FMath::Cos(SnappedAngle)), FMath::RoundToInt(FMath::Sin(SnappedAngle)));
	}

	FIntPoint GetNeighbour(const FIntPoint& Coordinates, const FIntPoint& Step)
	{
		return Coordinates + Step;
	}
}
