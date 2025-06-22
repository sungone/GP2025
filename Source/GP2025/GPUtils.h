#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"


#if !PLATFORM_ANDROID
void SaveRecastDebugGeometryToJson(const FRecastDebugGeometry& NavMeshGeometry, int32 NumTiles, const FString& FilePath);
bool ExtractNavMeshData(UWorld* World, const FString& PathName)
{
    auto* NavigationSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavigationSys) {
        UE_LOG(LogTemp, Error, TEXT("NavigationSystem not found!"));
        return false;
    }

    auto* NavData = NavigationSys->GetMainNavData();
    if (!NavData) {
        UE_LOG(LogTemp, Error, TEXT("NavData not found!"));
        return false;
    }

    ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
    if (!NavMesh) {
        UE_LOG(LogTemp, Error, TEXT("NavMesh not found!"));
        return false;
    }

    FString FullSavePath = FPaths::ProjectDir() + PathName;

    int32 NumTiles = NavMesh->GetNavMeshTilesCount();
    FRecastDebugGeometry NavMeshGeometry;

    NavMesh->GetDebugGeometry(NavMeshGeometry);
    SaveRecastDebugGeometryToJson(NavMeshGeometry, NumTiles, FullSavePath);
    return true;
}

void SaveRecastDebugGeometryToJson(const FRecastDebugGeometry& NavMeshGeometry, int32 NumTiles, const FString& FilePath)
{
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
    RootObject->SetNumberField(TEXT("NumTiles"), NumTiles);

    TArray<TSharedPtr<FJsonValue>> VerticesArray;
    VerticesArray.Reserve(NavMeshGeometry.MeshVerts.Num());

    for (const FVector& V : NavMeshGeometry.MeshVerts)
    {
        TArray<TSharedPtr<FJsonValue>> VertexTriple;
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.X));
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.Y));
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.Z));

        VerticesArray.Add(MakeShared<FJsonValueArray>(VertexTriple));
    }
    RootObject->SetArrayField(TEXT("Vertices"), VerticesArray);

    TArray<TSharedPtr<FJsonValue>> TrianglesArray;
    for (int32 AreaID = 0; AreaID < RECAST_MAX_AREAS; ++AreaID)
    {
        const TArray<int32>& IndicesInArea = NavMeshGeometry.AreaIndices[AreaID];
        for (int32 i = 0; i + 2 < IndicesInArea.Num(); i += 3)
        {
            TSharedPtr<FJsonObject> TriObject = MakeShared<FJsonObject>();
            TriObject->SetNumberField(TEXT("IndexA"), IndicesInArea[i]);
            TriObject->SetNumberField(TEXT("IndexB"), IndicesInArea[i + 1]);
            TriObject->SetNumberField(TEXT("IndexC"), IndicesInArea[i + 2]);
            TriObject->SetNumberField(TEXT("AreaID"), AreaID);

            TrianglesArray.Add(MakeShared<FJsonValueObject>(TriObject));
        }
    }
    RootObject->SetArrayField(TEXT("Triangles"), TrianglesArray);
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    if (FFileHelper::SaveStringToFile(OutputString, *FilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("NavMesh debug geometry saved to: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save NavMesh debug geometry to: %s"), *FilePath);
    }
}
#endif