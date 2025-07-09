// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class GP2025_API GPNavMeshExporter
{
public:
	static bool ExportNavMesh(UWorld* World, const FString& FilePath);
};
