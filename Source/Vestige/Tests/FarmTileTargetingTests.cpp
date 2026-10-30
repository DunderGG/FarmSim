// Copyright (C) 2026 David Bennehag and the Project Vestige contributors.
// SPDX-License-Identifier: GPL-3.0-or-later WITH AdditionRef-Vestige-Unreal-Exception

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"
#include "Farming/FarmTileTargeting.h"

// A namespace keeps the fixture's short name from hiding other declarations in unity builds.
namespace FarmTileTargetingTests
{
	/** A unit direction on the ground at AngleDegrees from the grid's X axis, toward its Y axis. */
	FVector2D DirectionAt(double AngleDegrees)
	{
		const double Radians = FMath::DegreesToRadians(AngleDegrees);
		return FVector2D(FMath::Cos(Radians), FMath::Sin(Radians));
	}

	TEST_CLASS(TileTargeting, "Vestige")
	{
		TEST_METHOD(TheEightDirectionsSnapToTheirSteps)
		{
			const FIntPoint Expected[] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
			for (int32 Direction = 0; Direction < UE_ARRAY_COUNT(Expected); ++Direction)
			{
				ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(Direction * 45.0)) == Expected[Direction]));
			}
		}

		TEST_METHOD(DirectionsSnapToTheNearerSideOfEachBoundary)
		{
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(22.4)) == FIntPoint(1, 0)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(22.6)) == FIntPoint(1, 1)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(67.4)) == FIntPoint(1, 1)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(67.6)) == FIntPoint(0, 1)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(-22.4)) == FIntPoint(1, 0)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(-22.6)) == FIntPoint(1, -1)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(180.0 - 22.4)) == FIntPoint(-1, 0)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(DirectionAt(180.0 + 22.4)) == FIntPoint(-1, 0)));
		}

		TEST_METHOD(TheLengthOfTheDirectionDoesNotMatter)
		{
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(FVector2D(0.2, 0.25)) == FIntPoint(1, 1)));
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(FVector2D(-300.0, 10.0)) == FIntPoint(-1, 0)));
		}

		TEST_METHOD(NoDirectionHasNoStep)
		{
			ASSERT_THAT(IsTrue(FarmTileTargeting::SnapToStep(FVector2D::ZeroVector) == FIntPoint::ZeroValue));
		}

		TEST_METHOD(TheNeighbourIsOneStepAwayInEachDirection)
		{
			const FIntPoint Tile(3, 3);
			for (int32 Direction = 0; Direction < 8; ++Direction)
			{
				const FIntPoint Step = FarmTileTargeting::SnapToStep(DirectionAt(Direction * 45.0));
				const FIntPoint Neighbour = FarmTileTargeting::GetNeighbour(Tile, Step);

				ASSERT_THAT(IsTrue(Neighbour != Tile));
				ASSERT_THAT(IsTrue(FMath::Abs(Neighbour.X - Tile.X) <= 1 && FMath::Abs(Neighbour.Y - Tile.Y) <= 1));
			}
			ASSERT_THAT(IsTrue(FarmTileTargeting::GetNeighbour(Tile, FIntPoint(-1, 1)) == FIntPoint(2, 4)));
		}
	};
}

#endif
