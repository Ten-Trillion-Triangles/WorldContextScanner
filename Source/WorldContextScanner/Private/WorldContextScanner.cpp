// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldContextScanner.h"
#include "WorldContextScannerStyle.h"
#include "WorldContextScannerCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "WorldContextScanLibrary.h"

static const FName WorldContextScannerTabName("WorldContextScanner");

#define LOCTEXT_NAMESPACE "FWorldContextScannerModule"

void FWorldContextScannerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FWorldContextScannerStyle::Initialize();
	FWorldContextScannerStyle::ReloadTextures();

	FWorldContextScannerCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FWorldContextScannerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FWorldContextScannerModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWorldContextScannerModule::RegisterMenus));
}

void FWorldContextScannerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWorldContextScannerStyle::Shutdown();

	FWorldContextScannerCommands::Unregister();
}

void FWorldContextScannerModule::PluginButtonClicked()
{
	UWorldContextScanLibrary::CreateEditorWidget("/Script/Blutility.EditorUtilityWidgetBlueprint'/WorldContextScanner/Scan_Menu.Scan_Menu'");
}

void FWorldContextScannerModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FWorldContextScannerCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FWorldContextScannerCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWorldContextScannerModule, WorldContextScanner)