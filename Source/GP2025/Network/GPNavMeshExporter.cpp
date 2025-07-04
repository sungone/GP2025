// GPNavMeshExporter.cpp

#include "GPNavMeshExporter.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

bool GPNavMeshExporter::ExportNavMesh(UWorld* World, const FString& FilePath)
{
    if (!World) return false;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavSys) return false;
    ARecastNavMesh* NavData = Cast<ARecastNavMesh>(NavSys->GetDefaultNavDataInstance());
    if (!NavData) return false;

    TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject());

    TArray<TSharedPtr<FJsonValue>> TrianglesJson;

    const int32 NumTiles = NavData->GetNavMeshTilesCount();
    for (int32 TileIndex = 0; TileIndex < NumTiles; ++TileIndex)
    {
        TArray<FNavPoly> Polys;
        if (!NavData->GetPolysInTile(TileIndex, Polys))
            continue;

        for (const FNavPoly& Poly : Polys)
        {
            TArray<FVector> Verts;
            if (!NavData->GetPolyVerts(Poly.Ref, Verts) || Verts.Num() < 3)
                continue;

            for (int32 i = 1; i + 1 < Verts.Num(); ++i)
            {
                TArray<TSharedPtr<FJsonValue>> TriVerts;

                auto AddVert = [&](const FVector& V)
                    {
                        TArray<TSharedPtr<FJsonValue>> Coord;
                        Coord.Add(MakeShareable(new FJsonValueNumber(V.X)));
                        Coord.Add(MakeShareable(new FJsonValueNumber(V.Y)));
                        Coord.Add(MakeShareable(new FJsonValueNumber(V.Z)));
                        TriVerts.Add(MakeShareable(new FJsonValueArray(Coord)));
                    };

                AddVert(Verts[0]);
                AddVert(Verts[i]);
                AddVert(Verts[i + 1]);

                TrianglesJson.Add(MakeShareable(new FJsonValueArray(TriVerts)));
            }
        }
    }

    Root->SetArrayField(TEXT("Triangles"), TrianglesJson);

    FString Output;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    if (!FJsonSerializer::Serialize(Root.ToSharedRef(), Writer))
        return false;

    const FString Dir = FPaths::ProjectSavedDir() / TEXT("NavMesh");
    FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Dir);
    const FString FullPath = Dir / FilePath;

    return FFileHelper::SaveStringToFile(Output, *FullPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}