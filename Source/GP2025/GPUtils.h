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
        BoxData.Name = StaticMeshActor->GetActorLabel();

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
