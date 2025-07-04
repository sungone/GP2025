#include "pch.h"
#include "NavMesh.h"
#include "SessionManager.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

std::optional<NavMesh> NavMesh::LoadFromJson(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return std::nullopt;

    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError())
        return std::nullopt;

    if (!doc.HasMember("Triangles") || !doc["Triangles"].IsArray())
        return std::nullopt;

    NavMesh mesh;

    const auto& tris = doc["Triangles"].GetArray();
    for (const auto& tri : tris)
    {
        if (!tri.IsArray() || tri.Size() != 3)
            continue;

        FVector v0(tri[0][0].GetFloat(), tri[0][1].GetFloat(), tri[0][2].GetFloat());
        FVector v1(tri[1][0].GetFloat(), tri[1][1].GetFloat(), tri[1][2].GetFloat());
        FVector v2(tri[2][0].GetFloat(), tri[2][1].GetFloat(), tri[2][2].GetFloat());

        int i0 = static_cast<int>(mesh.vertices.size());
        mesh.vertices.push_back(v0);
        mesh.vertices.push_back(v1);
        mesh.vertices.push_back(v2);

        mesh.triangles.emplace_back(i0, i0 + 1, i0 + 2);
    }

    mesh.BuildAdjacencyCSR();

    return mesh;
}

void NavMesh::BuildAdjacencyCSR()
{
    int triCount = static_cast<int>(triangles.size());

    using Edge = std::pair<int, int>;
    auto MakeEdge = [](int a, int b) {
        return std::make_pair(std::min(a, b), std::max(a, b));
        };
    struct EdgeHash {
        size_t operator()(const Edge& e) const {
            return std::hash<int>()(e.first) ^ (std::hash<int>()(e.second) << 1);
        }
    };

    std::unordered_map<Edge, std::vector<int>, EdgeHash> edgeMap;

    for (int i = 0; i < triCount; ++i) {
        const auto& tri = triangles[i];
        edgeMap[MakeEdge(tri.a, tri.b)].push_back(i);
        edgeMap[MakeEdge(tri.b, tri.c)].push_back(i);
        edgeMap[MakeEdge(tri.c, tri.a)].push_back(i);
    }

    std::vector<std::vector<int>> neighbors(triCount);
    for (const auto& [edge, triList] : edgeMap) {
        if (triList.size() == 2) {
            int t1 = triList[0];
            int t2 = triList[1];
            neighbors[t1].push_back(t2);
            neighbors[t2].push_back(t1);
        }
    }

    nbrOffsets.clear();
    nbrOffsets.reserve(triCount + 1);
    nbrIndices.clear();

    nbrOffsets.push_back(0);
    for (int i = 0; i < triCount; ++i) {
        nbrIndices.insert(nbrIndices.end(), neighbors[i].begin(), neighbors[i].end());
        nbrOffsets.push_back(static_cast<int>(nbrIndices.size()));
    }
}

int NavMesh::FindIdxFromPos(const FVector& pos) const
{
    auto Sign = [](const FVector& p1, const FVector& p2, const FVector& p3)
        {
            return (p1.X - p3.X) * (p2.Y - p3.Y) - (p2.X - p3.X) * (p1.Y - p3.Y);
        };

    for (int i = 0; i < triangles.size(); ++i) {
        const auto& tri = triangles[i];
        const FVector& A = vertices[tri.a];
        const FVector& B = vertices[tri.b];
        const FVector& C = vertices[tri.c];

        float d1 = Sign(pos, A, B);
        float d2 = Sign(pos, B, C);
        float d3 = Sign(pos, C, A);

        bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        if (!(hasNeg && hasPos))
            return i;
    }

    return -1;
}

FVector NavMesh::GetRandomPosition() const
{
    if (triangles.empty()) return FVector::ZeroVector;

    int idx = RandomUtils::GetRandomInt(0, static_cast<int>(triangles.size()) - 1);
    const auto& tri = triangles[idx];

    const FVector& A = vertices[tri.a];
    const FVector& B = vertices[tri.b];
    const FVector& C = vertices[tri.c];

    float u = RandomUtils::GetRandomFloat(0.f, 1.f);
    float v = RandomUtils::GetRandomFloat(0.f, 1.f);
    if (u + v > 1.f) {
        u = 1.f - u;
        v = 1.f - v;
    }

    FVector P = A + (B - A) * u + (C - A) * v;
    P.Z += 90.f;
    return P;
}

std::vector<int> NavMesh::FindPath(int startTri, int goalTri) const
{
    if (startTri < 0 || goalTri < 0 || startTri >= triangles.size() || goalTri >= triangles.size())
        return {};

    std::vector<int> prev(triangles.size(), -1);
    std::vector<int> q;
    q.reserve(triangles.size());
    int qh = 0;
    q.push_back(startTri);
    prev[startTri] = startTri;

    while (qh < q.size()) {
        int cur = q[qh++];
        if (cur == goalTri) break;
        for (int k = nbrOffsets[cur]; k < nbrOffsets[cur + 1]; ++k) {
            int nb = nbrIndices[k];
            if (prev[nb] == -1) {
                prev[nb] = cur;
                q.push_back(nb);
            }
        }
    }

    if (prev[goalTri] == -1) return {};
    std::vector<int> path;
    for (int at = goalTri; at != startTri; at = prev[at])
        path.push_back(at);
    path.push_back(startTri);
    std::reverse(path.begin(), path.end());
    return path;
}