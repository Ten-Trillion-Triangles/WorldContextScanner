// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "WorldContextScannerStyle.h"

class FWorldContextScannerCommands : public TCommands<FWorldContextScannerCommands>
{
public:

	FWorldContextScannerCommands()
		: TCommands<FWorldContextScannerCommands>(TEXT("WorldContextScanner"), NSLOCTEXT("Contexts", "WorldContextScanner", "WorldContextScanner Plugin"), NAME_None, FWorldContextScannerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
