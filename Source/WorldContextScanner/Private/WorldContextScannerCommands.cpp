// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldContextScannerCommands.h"

#define LOCTEXT_NAMESPACE "FWorldContextScannerModule"

void FWorldContextScannerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "WorldContextScanner", "Execute WorldContextScanner action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
