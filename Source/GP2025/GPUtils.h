#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

/**
 * �ٿ�� �ڽ�(AABB) ������ ����ü
 */
struct FBoundingBoxData
{
    FString Name;
    FVector Min;
    FVector Max;
};


/**
 * Static Mesh�� �ٿ�� �ڽ�(AABB) ������ ���� �Լ�
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
        // ������ ȯ�濡���� ��ȿ�� �Լ�
        BoxData.Name = StaticMeshActor->GetActorLabel();
#else
        // ��Ű¡�� ���忡���� Actor �̸��� �״�� ���
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
 * JSON ���� ���� �Լ�
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
 * ���� �Լ�: �ٿ�� �ڽ� ������ ���� �� ����
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
    // 1) �ֻ��� JSON ������Ʈ ����
    TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
    // NavMesh Ÿ�� ���� ���
    RootObject->SetNumberField(TEXT("NumTiles"), NumTiles);

    //
    // 2) ���ؽ� �迭 (MeshVerts)
    //
    TArray<TSharedPtr<FJsonValue>> VerticesArray;
    VerticesArray.Reserve(NavMeshGeometry.MeshVerts.Num());

    for (const FVector& V : NavMeshGeometry.MeshVerts)
    {
        // ��: [ X, Y, Z ] ���·� ���
        TArray<TSharedPtr<FJsonValue>> VertexTriple;
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.X));
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.Y));
        VertexTriple.Add(MakeShared<FJsonValueNumber>(V.Z));

        VerticesArray.Add(MakeShared<FJsonValueArray>(VertexTriple));
    }
    // JSON �ʵ�: "Vertices"
    RootObject->SetArrayField(TEXT("Vertices"), VerticesArray);

    //
    // 3) �ﰢ�� �ε��� (AreaIndices)
    //
    // NavMesh������ ����(Area)���� �ε����� �и��Ǿ� �ֽ��ϴ�.
    // �� AreaID���� 3���� ���� �ﰢ���� �����ϹǷ�, Triangles �迭�� ��Ƽ� �����ϰڽ��ϴ�.
    TArray<TSharedPtr<FJsonValue>> TrianglesArray;

    // RECAST_MAX_AREAS = 64(�⺻��)  
    // �����Ϳ��� ��ϵǴ� NavArea (DefaultArea, Jump, Lava ��)�� �ش��ϴ� �ε��� ����� ���� �������.
    for (int32 AreaID = 0; AreaID < RECAST_MAX_AREAS; ++AreaID)
    {
        const TArray<int32>& IndicesInArea = NavMeshGeometry.AreaIndices[AreaID];
        // �ε����� 3���� �ϳ��� �ﰢ��
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

    // (�ɼ�) OffMeshLinks, PolyEdges, NavMeshEdges � �ʿ��ϴٸ� �߰� ����ȭ ����

    //
    // 4) JSON �� ���ڿ� ��ȯ �� ���Ϸ� ����
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
