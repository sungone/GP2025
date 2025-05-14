#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

/**
 * 바운딩 박스(AABB) 데이터 구조체
 */
struct FBoundingBoxData
{
    FString Name;
    FVector Min;
    FVector Max;
};


/**
 * Static Mesh의 바운딩 박스(AABB) 데이터 추출 함수
 */
TArray<FBoundingBoxData> ExtractBoundingBoxData(ULevel* Level)
{
    TArray<FBoundingBoxData> BoundingBoxDataArray;

    for (AActor* Actor : Level->Actors)
    {
        AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(Actor);
        if (!StaticMeshActor) continue;

        UStaticMeshComponent* MeshComponent = StaticMeshActor->GetStaticMeshComponent();
        if (!MeshComponent) continue;

        UStaticMesh* StaticMesh = MeshComponent->GetStaticMesh();
        if (!StaticMesh) continue;

        FBoundingBoxData BoxData;

#if WITH_EDITOR
        // 에디터 환경에서만 유효한 함수
        BoxData.Name = StaticMeshActor->GetActorLabel();
#else
        // 패키징된 빌드에서는 Actor 이름을 그대로 사용
        BoxData.Name = StaticMeshActor->GetName();
#endif

        USceneComponent* RootComponent = StaticMeshActor->GetRootComponent();
        if (!RootComponent) continue;

        FVector Location = RootComponent->GetRelativeLocation();
        FRotator Rotation = RootComponent->GetRelativeRotation();
        FVector Scale = RootComponent->GetRelativeScale3D();
        FTransform ActorTransform = FTransform(Rotation, Location, Scale);

        FBox LocalAABB = StaticMesh->GetBoundingBox();

        FVector Min = ActorTransform.TransformPosition(LocalAABB.Min);
        FVector Max = ActorTransform.TransformPosition(LocalAABB.Max);

        BoxData.Min = Min;
        BoxData.Max = Max;

        UE_LOG(LogTemp, Log, TEXT("%s - World BoundingBox Min(%f, %f, %f) Max(%f, %f, %f)"),
            *BoxData.Name,
            BoxData.Min.X, BoxData.Min.Y, BoxData.Min.Z,
            BoxData.Max.X, BoxData.Max.Y, BoxData.Max.Z);

        BoundingBoxDataArray.Add(BoxData);
    }

    return BoundingBoxDataArray;
}


/**
 * JSON 파일 저장 함수
 */
void SaveBoundingBoxDataToJson(const TArray<FBoundingBoxData>& BoundingBoxData, const FString& FilePath)
{
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> BoundingBoxArray;

    for (const FBoundingBoxData& Data : BoundingBoxData)
    {
        TSharedPtr<FJsonObject> BoundingBoxObject = MakeShared<FJsonObject>();

        BoundingBoxObject->SetStringField("Name", Data.Name);

        TSharedPtr<FJsonObject> BoundingBoxValues = MakeShared<FJsonObject>();
        BoundingBoxValues->SetNumberField("MinX", Data.Min.X);
        BoundingBoxValues->SetNumberField("MinY", Data.Min.Y);
        BoundingBoxValues->SetNumberField("MinZ", Data.Min.Z);
        BoundingBoxValues->SetNumberField("MaxX", Data.Max.X);
        BoundingBoxValues->SetNumberField("MaxY", Data.Max.Y);
        BoundingBoxValues->SetNumberField("MaxZ", Data.Max.Z);

        BoundingBoxObject->SetObjectField("BoundingBox", BoundingBoxValues);
        BoundingBoxArray.Add(MakeShared<FJsonValueObject>(BoundingBoxObject));
    }

    RootObject->SetArrayField("BoundingBoxes", BoundingBoxArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    FFileHelper::SaveStringToFile(OutputString, *FilePath);

    UE_LOG(LogTemp, Log, TEXT("Bounding box data saved to: %s"), *FilePath);
}

/**
 * 실행 함수: 바운딩 박스 데이터 추출 및 저장
 */
void ExportLevelBoundingBoxData(ULevel* Level, const FString& PathName)
{
    FString SavePath = FPaths::ProjectDir() + PathName;

    TArray<FBoundingBoxData> BoundingBoxData = ExtractBoundingBoxData(Level);
    SaveBoundingBoxDataToJson(BoundingBoxData, SavePath);
}

#include "NavMesh/RecastNavMesh.h"
#include "NavigationSystem.h"

void SaveRecastDebugGeometryToJson(const FRecastDebugGeometry& NavMeshGeometry, int32 NumTiles, const FString& FilePath);
void ExtractNavMeshData(UWorld* World, const FString& PathName)
{
    auto* NavigationSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavigationSys) {
        UE_LOG(LogTemp, Error, TEXT("NavigationSystem not found!"));
        return;
    }

    auto* NavData = NavigationSys->GetMainNavData();
    if (!NavData) {
        UE_LOG(LogTemp, Error, TEXT("NavData not found!"));
        return;
    }

    ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
    if (!NavMesh) {
        UE_LOG(LogTemp, Error, TEXT("NavMesh not found!"));
        return;
    }

    FString FullSavePath = FPaths::ProjectDir() + PathName;

    int32 NumTiles = NavMesh->GetNavMeshTilesCount();
    UE_LOG(LogTemp, Log, TEXT("NavMesh Tiles: %d"), NumTiles);

    for (int32 TileIndex = 0; TileIndex < NumTiles; ++TileIndex)
    {
        FRecastDebugGeometry NavMeshGeometry;

        if (NavMesh->GetDebugGeometryForTile(NavMeshGeometry, TileIndex))
        {
            SaveRecastDebugGeometryToJson(NavMeshGeometry, TileIndex, FullSavePath);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get debug geometry for tile index %d"), TileIndex);
        }
    }
}

void SaveRecastDebugGeometryToJson(const FRecastDebugGeometry& NavMeshGeometry, int32 NumTiles, const FString& FilePath)
{
    // 1) 최상위 JSON 오브젝트 생성
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
    // NavMesh 타일 개수 기록
    RootObject->SetNumberField(TEXT("NumTiles"), NumTiles);

    //
    // 2) 버텍스 배열 (MeshVerts)
    //
    TArray<TSharedPtr<FJsonValue>> VerticesArray;
    VerticesArray.Reserve(NavMeshGeometry.MeshVerts.Num());

    for (const FVector& V : NavMeshGeometry.MeshVerts)
    {
        // 예: [ X, Y, Z ] 형태로 기록
        TArray<TSharedPtr<FJsonValue>> VertexTriple;
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.X));
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.Y));
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.Z));

        VerticesArray.Add(MakeShared<FJsonValueArray>(VertexTriple));
    }
    // JSON 필드: "Vertices"
    RootObject->SetArrayField(TEXT("Vertices"), VerticesArray);

    //
    // 3) 삼각형 인덱스 (AreaIndices)
    //
    // NavMesh에서는 영역(Area)마다 인덱스가 분리되어 있습니다.
    // 각 AreaID마다 3개씩 묶어 삼각형을 구성하므로, Triangles 배열로 모아서 저장하겠습니다.
    TArray<TSharedPtr<FJsonValue>> TrianglesArray;

    // RECAST_MAX_AREAS = 64(기본값)  
    // 에디터에서 등록되는 NavArea (DefaultArea, Jump, Lava 등)에 해당하는 인덱스 목록이 각각 들어있음.
    for (int32 AreaID = 0; AreaID < RECAST_MAX_AREAS; ++AreaID)
    {
        const TArray<int32>& IndicesInArea = NavMeshGeometry.AreaIndices[AreaID];
        // 인덱스는 3개씩 하나의 삼각형
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

    // (옵션) OffMeshLinks, PolyEdges, NavMeshEdges 등도 필요하다면 추가 직렬화 가능

    //
    // 4) JSON → 문자열 변환 후 파일로 저장
    //
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
