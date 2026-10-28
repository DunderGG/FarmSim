// Copyright Epic Games, Inc. All Rights Reserved.

#include "VestigeGameMode.h"

#include "FarmHUD.h"

AVestigeGameMode::AVestigeGameMode()
{
	HUDClass = AFarmHUD::StaticClass();
}
