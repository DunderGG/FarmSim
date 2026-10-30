// Copyright Epic Games, Inc. All Rights Reserved.

#include "Template/VestigeGameMode.h"

#include "UI/FarmHUD.h"

AVestigeGameMode::AVestigeGameMode()
{
	HUDClass = AFarmHUD::StaticClass();
}
