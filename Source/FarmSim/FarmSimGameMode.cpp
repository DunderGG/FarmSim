// Copyright Epic Games, Inc. All Rights Reserved.

#include "FarmSimGameMode.h"

#include "FarmHUD.h"

AFarmSimGameMode::AFarmSimGameMode()
{
	HUDClass = AFarmHUD::StaticClass();
}
