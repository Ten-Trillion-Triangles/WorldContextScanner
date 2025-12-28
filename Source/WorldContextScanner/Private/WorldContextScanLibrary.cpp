// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldContextScanLibrary.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "EngineUtils.h"
#include "K2Node_FunctionEntry.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Async/Async.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Linux/LinuxPlatformApplicationMisc.h"
#include "UObject/UnrealTypePrivate.h"


TMap<FString, FScanStringArray> UWorldContextScanLibrary::ScanUAssets(FName Path, bool bRecursive)
{

	//Boilerplate to get our asset registry and data to hold our assets we'll be loading.
	TMap<FString, FScanStringArray> Results;
	TArray<FAssetData> AssetList;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	

	
	if(!AssetRegistryModule.IsValid())
	{
		return Results; //Bail if we have a nullptr for the asset registry
	}

	//Get all our assets recursively based on the starting path supplied.
	bool IsValid = AssetRegistry.GetAssetsByPath(Path, AssetList, true);

	if(!IsValid)
	{
		return Results; //Bail if we could not get the asset data.
	}

	for(auto& Elem: AssetList)
	{
		UObject* AssetRef = Elem.GetAsset(); //Load the asset into memory.

		if(AssetRef)
		{
			UE_LOG(LogTemp, Log, TEXT("Loaded asset: %s"), *Elem.AssetName.ToString()); //Test assest name.
			FString AssetName = Elem.AssetName.ToString(); 
			FScanStringArray PinList;

			//Boiler plate to get our raw function data.
			UClass* Class = AssetRef->GetClass();
			UBlueprint* Blueprint = Cast<UBlueprint>(AssetRef);
			PinList.StringArray.Empty(); //Clear PinList before proceeding.
			if(Blueprint != nullptr)
			{
				//We need to find every single graph on this blueprint in order to locate raw functions.
				TArray<UEdGraph*> AllGraphs;
				Blueprint->GetAllGraphs(AllGraphs);

				for(UEdGraph* Graph : AllGraphs)
				{
					//Split down to the raw nodes.
					TArray<UK2Node_FunctionEntry*> FunctionEntries;

					for(UEdGraphNode* Node : Graph->Nodes)
					{
						//We need to test each node to find the entry point. The entry point is the declaration and params.
						UK2Node_FunctionEntry* FunctionEntry = Cast<UK2Node_FunctionEntry>(Node);
						if(FunctionEntry != nullptr)
						{
							FString FunctionName = FunctionEntry->GetFullName();
							UE_LOG(LogTemp, Log, TEXT("Iterating over function: %s"), *FunctionName);

							//Unwind our pins next and begin looking for the cursed __WorldContext pin from hell.
							for (UEdGraphPin* Pin : FunctionEntry->Pins)
							{
								FString PinName = Pin->PinName.ToString();
								UE_LOG(LogTemp, Log, TEXT("Checking function pin: %s"), *PinName);
								if(Pin->PinName == "__WorldContext")
								{
									//If the node exists and is visible it's uh oh time.
									if(Node->FindPin(TEXT("__WorldContext")) && Pin->bHidden == false)
									{
										PinList.StringArray.Add(FunctionName); //Mark this function as bad.
										UE_LOG(LogTemp, Log, TEXT("Discovered bad __WorldContext pin"));
									}
									
								}
							}

							if(PinList.StringArray.Num() > 0)
							{
								Results.Add(AssetName, PinList); //Add the data of this function library to our results.
							}
						}
					}
				}
			}
		}
	}

	
	return Results;
}



void UWorldContextScanLibrary::ScanFunctions(FName Path, bool bRecursive)
{

	//Required boiler plate to unwind the TMap.
	TMap<FString, FScanStringArray> Results = ScanUAssets(Path, bRecursive);

	
	//We need to spin this off to a separate thread because this can get very expensive.
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [Path, bRecursive, Results] ()
	{

		//Lock down this function for thread safety.
		FCriticalSection Mutex;
		FScopeLock Lock(&Mutex);

		
		
		TArray<FString> Keys;
		Results.GetKeys(Keys);
		FString Output = "------__WorldContext results-------"; //Header for the list.

		//Unravel the list of pins and print out each one that exists on this function that is faulty.
		for(auto& Elem: Keys)
		{
			FScanStringArray Pins = Results.FindRef(Elem);
			
			//Function name does not denote the library it belongs to. 
			FString FunctionFormatter = "\nFunction Name:" + Elem;
			Output = Output + FunctionFormatter;

			for(auto& PinsFound: Pins.StringArray)
			{
				Output = Output + "\n" + PinsFound; //We want each pin to be it's own line here.
			}
		}


		//Return to game thread for safety reasons before we dump this data.
		AsyncTask(ENamedThreads::GameThread, [Output] ()
		{

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Output);
			CopyClipBoard(Output);

		});
	});
}



void UWorldContextScanLibrary::CopyClipBoard(FString TargetString)
{
	const TCHAR* TConvert = *TargetString;
	FGenericPlatformMisc::ClipboardCopy(TConvert);
	
}




void UWorldContextScanLibrary::CreateEditorWidget(FString FilePath)
{
	

	const TCHAR* TConvert = *FilePath;

	if(!FilePath.IsEmpty())
	{
		UEditorUtilityWidgetBlueprint* Blueprint = LoadObject<UEditorUtilityWidgetBlueprint>(NULL, TConvert);

		if(!Blueprint)
		{
			//Bail and return here if its not valid?
			return;
		}

		// Create an instance of the blueprint and spawn it.
		UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		EditorUtilitySubsystem->SpawnAndRegisterTab(Blueprint);

		
		
	}

	
	
}
