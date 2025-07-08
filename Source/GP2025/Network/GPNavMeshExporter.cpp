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

    // 1) 준비: 폴리곤 참조와 인덱스 매핑
    TArray<uint64>   AllPolyRefs;
    TMap<uint64, int32> PolyRefToIndex;

    // 2) 전역 정점 풀 및 폴리곤, 삼각형 JSON 준비
    TMap<FVector, int32> VertexToIndex;
    TArray<FVector>      VertexList;
    auto GetOrAddVertexIndex = [&](const FVector& V) -> int32 {
        if (int32* Found = VertexToIndex.Find(V))
            return *Found;
        int32 NewIdx = VertexList.Num();
        VertexList.Add(V);
        VertexToIndex.Add(V, NewIdx);
        return NewIdx;
        };

    TArray<TArray<int32>>       Polygons;      // 폴리곤별 정점 인덱스
    TArray<TSharedPtr<FJsonValue>> TrianglesJson; // 삼각형 좌표 JSON

    // 3) 타일 순회: 폴리곤·정점·삼각형 수집
    const int32 NumTiles = NavData->GetNavMeshTilesCount();
    for (int32 TileIdx = 0; TileIdx < NumTiles; ++TileIdx)
    {
        TArray<FNavPoly> Polys;
        if (!NavData->GetPolysInTile(TileIdx, Polys))
            continue;

        for (const FNavPoly& Poly : Polys)
        {
            // (a) PolyRef 기록
            uint64 Ref = Poly.Ref;
            int32 PolyIdx = AllPolyRefs.Num();
            AllPolyRefs.Add(Ref);
            PolyRefToIndex.Add(Ref, PolyIdx);

            // (b) 폴리곤 정점 가져오기
            TArray<FVector> Verts;
            if (!NavData->GetPolyVerts(Ref, Verts) || Verts.Num() < 3)
                continue;

            // (c) 폴리곤 인덱스 리스트 생성
            TArray<int32> PolyIndices;
            PolyIndices.Reserve(Verts.Num());
            for (const FVector& V : Verts)
            {
                PolyIndices.Add(GetOrAddVertexIndex(V));
            }
            Polygons.Add(MoveTemp(PolyIndices));

            // (d) 볼록 폴리곤 삼각 분할하여 JSON에 저장
            for (int32 i = 1; i + 1 < Verts.Num(); ++i)
            {
                TArray<TSharedPtr<FJsonValue>> TriVerts;
                auto AddVert = [&](const FVector& VV) {
                    TArray<TSharedPtr<FJsonValue>> Coord;
                    Coord.Add(MakeShareable(new FJsonValueNumber(VV.X)));
                    Coord.Add(MakeShareable(new FJsonValueNumber(VV.Y)));
                    Coord.Add(MakeShareable(new FJsonValueNumber(VV.Z)));
                    TriVerts.Add(MakeShareable(new FJsonValueArray(Coord)));
                    };
                AddVert(Verts[0]);
                AddVert(Verts[i]);
                AddVert(Verts[i + 1]);
                TrianglesJson.Add(MakeShareable(new FJsonValueArray(TriVerts)));
            }
        }
    }

    // 4) 폴리곤별 이웃 정보 추출 (타일 내부 + 외부)
    TArray<TSharedPtr<FJsonValue>> NeighborsJson;
    NeighborsJson.Reserve(AllPolyRefs.Num());
    for (int32 idx = 0; idx < AllPolyRefs.Num(); ++idx)
    {
        uint64 PolyRef = AllPolyRefs[idx];
        TArray<uint64> Linked;
        NavData->GetPolyNeighbors(PolyRef, Linked);

        TArray<TSharedPtr<FJsonValue>> Arr;
        Arr.Reserve(Linked.Num());
        for (uint64 NeiRef : Linked)
        {
            if (int32* NeiIdx = PolyRefToIndex.Find(NeiRef))
            {
                Arr.Add(MakeShareable(new FJsonValueNumber(*NeiIdx)));
            }
        }
        NeighborsJson.Add(MakeShareable(new FJsonValueArray(Arr)));
    }

    // 5) JSON 배열로 변환: Vertices, Polygons, Triangles, Neighbors
    TSharedPtr<FJsonObject> Root = MakeShareable(new FJsonObject());

    // Vertices
    TArray<TSharedPtr<FJsonValue>> VertexJson;
    VertexJson.Reserve(VertexList.Num());
    for (const FVector& V : VertexList)
    {
        TArray<TSharedPtr<FJsonValue>> Coord;
        Coord.Add(MakeShareable(new FJsonValueNumber(V.X)));
        Coord.Add(MakeShareable(new FJsonValueNumber(V.Y)));
        Coord.Add(MakeShareable(new FJsonValueNumber(V.Z)));
        VertexJson.Add(MakeShareable(new FJsonValueArray(Coord)));
    }
    Root->SetArrayField(TEXT("Vertices"), VertexJson);

    // Polygons
    TArray<TSharedPtr<FJsonValue>> PolygonsJson;
    PolygonsJson.Reserve(Polygons.Num());
    for (const TArray<int32>& PolyVerts : Polygons)
    {
        TArray<TSharedPtr<FJsonValue>> PolyJson;
        PolyJson.Reserve(PolyVerts.Num());
        for (int32 Idx : PolyVerts)
        {
            PolyJson.Add(MakeShareable(new FJsonValueNumber(Idx)));
        }
        PolygonsJson.Add(MakeShareable(new FJsonValueArray(PolyJson)));
    }
    Root->SetArrayField(TEXT("Polygons"), PolygonsJson);

    // Triangles (기존 삼각형 분할)
    Root->SetArrayField(TEXT("Triangles"), TrianglesJson);

    // Neighbors (폴리곤 이웃 인덱스)
    Root->SetArrayField(TEXT("Neighbors"), NeighborsJson);

    // 6) 직렬화 & 파일 저장
    FString Output;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
    if (!FJsonSerializer::Serialize(Root.ToSharedRef(), Writer))
        return false;

    const FString Dir = FPaths::ProjectSavedDir() / TEXT("NavMesh");
    const FString FullPath = Dir / FilePath;
    FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Dir);
    return FFileHelper::SaveStringToFile(Output, *FullPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}
