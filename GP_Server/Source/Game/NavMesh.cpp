#include "pch.h"
#include "NavMesh.h"


bool LoadNavMeshFromJson(const std::string& filePath, NavMeshData& OutNavMeshData, bool isPrint)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LOG(LogType::Warning, std::format("Failed to open file: {}", filePath));
        return false;
    }

    std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());


    if (doc.HasParseError())
    {
        std::cerr << "[LoadNavMeshFromJson] JSON parse error: "
            << GetParseError_En(doc.GetParseError())
            << " at offset " << doc.GetErrorOffset() << std::endl;
        return false;
    }

    if (doc.HasMember("NumTiles") && doc["NumTiles"].IsInt())
    {
        OutNavMeshData.NumTiles = doc["NumTiles"].GetInt();
    }

    if (doc.HasMember("Vertices") && doc["Vertices"].IsArray())
    {
        const auto& verts = doc["Vertices"];
        OutNavMeshData.Vertices.reserve(verts.Size());

        for (auto i = 0; i < verts.Size(); i++)
        {
            const auto& vertVal = verts[i];
            if (vertVal.IsArray() && vertVal.Size() == 3)
            {
                FVector v;
                v.X = vertVal[0].GetFloat();
                v.Y = vertVal[1].GetFloat();
                v.Z = vertVal[2].GetFloat();
                OutNavMeshData.Vertices.push_back(v);
            }
        }
    }

    if (doc.HasMember("Triangles") && doc["Triangles"].IsArray())
    {
        const auto& tris = doc["Triangles"];
        OutNavMeshData.Triangles.reserve(tris.Size());

        for (auto i = 0; i < tris.Size(); i++)
        {
            const auto& triVal = tris[i];
            if (triVal.IsObject())
            {
                NavTriangle t;
                t.IndexA = triVal.HasMember("IndexA") && triVal["IndexA"].IsInt() ? triVal["IndexA"].GetInt() : -1;
                t.IndexB = triVal.HasMember("IndexB") && triVal["IndexB"].IsInt() ? triVal["IndexB"].GetInt() : -1;
                t.IndexC = triVal.HasMember("IndexC") && triVal["IndexC"].IsInt() ? triVal["IndexC"].GetInt() : -1;
                t.AreaID = triVal.HasMember("AreaID") && triVal["AreaID"].IsInt() ? triVal["AreaID"].GetInt() : 0;

                OutNavMeshData.Triangles.push_back(t);
            }
        }
    }

    if (isPrint)
    {
        OutNavMeshData.Print();
    }

    return true;
}