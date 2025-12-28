// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldContextScanLibrary.generated.h"

/**
 * 
 */

//Struct that holds a wrapped up array of strings.
USTRUCT(BlueprintType)
struct FScanStringArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> StringArray;
	
};

UCLASS()
class WORLDCONTEXTSCANNER_API UWorldContextScanLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Scans all UAssets and returns which blueprints and which functions inside them have __WorldContext pins.
	 * @param Path Path to begin scanning from.
	 * @param bRecursive If true paths will be deduced recursively and scanned.
	 * @return Returns a tmap that lists each asset found and each function containing this cursed pin from hell.
	 */
	static TMap<FString, FScanStringArray> ScanUAssets(FName Path, bool bRecursive);

	/**
	 * Given an input path load all UAssets found and reports all functions that have the forbidden pin within them.
	 * @param Path File path formatted as an unreal engine asset path: /game/blueprints/meshes 
	 * @param bRecursive If true, the scan for paths will become recursive.
	 * @note The results of this scan will be copied to your clipboard.
	 */
	UFUNCTION(BlueprintCallable)
	static void ScanFunctions(FName Path, bool bRecursive);

	/**
	 * Copies a string to the clipboard.
	 * @param TargetString The string we wish to copy.
	 */
	static void CopyClipBoard(FString TargetString);

	/** Creates an editor widget and adds it to the screen.
	 *@param FilePath: The UE4 Asset path of the widget.
	 */
	UFUNCTION(BlueprintCallable)
	static void CreateEditorWidget(FString FilePath);
};
