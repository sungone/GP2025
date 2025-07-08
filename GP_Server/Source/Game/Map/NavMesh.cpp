#include "pch.h"
#include "NavMesh.h"
#include "SessionManager.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

std::optional<NavMesh> NavMesh::LoadFromJson(const std::string& filePath)
{
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) return std::nullopt;
    std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    rapidjson::Document doc;
    doc.Parse(str.c_str());
    if (doc.HasParseError()
        || !doc.HasMember("Vertices") || !doc["Vertices"].IsArray()
        || !doc.HasMember("Polygons") || !doc["Polygons"].IsArray()
        || !doc.HasMember("Neighbors") || !doc["Neighbors"].IsArray())
    {
        return std::nullopt;
    }

    NavMesh mesh;

    const auto& vArr = doc["Vertices"].GetArray();
    mesh.vertices.reserve(vArr.Size());
    for (const auto& vj : vArr)
    {
        const auto& a = vj.GetArray();
        mesh.vertices.emplace_back(
            a[0].GetFloat(),
            a[1].GetFloat(),
            a[2].GetFloat()
        );
    }

    const auto& pArr = doc["Polygons"].GetArray();
    mesh.polygons.reserve(pArr.Size());
    for (const auto& pj : pArr)
    {
        const auto& idxs = pj.GetArray();
        std::vector<int> poly;
        poly.reserve(idxs.Size());
        for (const auto& xi : idxs)
            poly.push_back(xi.GetInt());
        mesh.polygons.push_back(std::move(poly));
    }

    const auto& nArr = doc["Neighbors"].GetArray();
    mesh.neighbors.reserve(nArr.Size());
    for (const auto& nj : nArr)
    {
        const auto& ni = nj.GetArray();
        std::vector<int> row;
        row.reserve(ni.Size());
        for (const auto& xi : ni)
            row.push_back(xi.GetInt());
        mesh.neighbors.push_back(std::move(row));
    }

    return mesh;
}

static bool PointInPoly(const FVector& P, const std::vector<int>& poly,
    const std::vector<FVector>& verts)
{
    bool inside = false;
    int n = poly.size();
    for (int i = 0, j = n - 1; i < n; j = i++)
    {
        const FVector& vi = verts[poly[i]];
        const FVector& vj = verts[poly[j]];
        bool intersect = ((vi.Y > P.Y) != (vj.Y > P.Y)) &&
            (P.X < (vj.X - vi.X) * (P.Y - vi.Y) / (vj.Y - vi.Y) + vi.X);
        if (intersect) inside = !inside;
    }
    return inside;
}

int NavMesh::FindIdxFromPos(const FVector& pos) const
{
    for (int i = 0; i < (int)polygons.size(); ++i)
    {
        if (PointInPoly(pos, polygons[i], vertices))
            return i;
    }
    return -1;
}

std::vector<int> NavMesh::FindPath(int startPoly, int goalPoly) const
{
    int N = (int)polygons.size();
    if (startPoly < 0 || goalPoly < 0 || startPoly >= N || goalPoly >= N)
        return {};

    std::vector<int> prev(N, -1);
    std::deque<int> queue;
    queue.push_back(startPoly);
    prev[startPoly] = startPoly;

    while (!queue.empty())
    {
        int cur = queue.front(); queue.pop_front();
        if (cur == goalPoly) break;

        for (int nb : neighbors[cur])
        {
            if (prev[nb] == -1)
            {
                prev[nb] = cur;
                queue.push_back(nb);
            }
        }
    }

    if (prev[goalPoly] == -1) return {};

    std::vector<int> path;
    for (int at = goalPoly; at != startPoly; at = prev[at])
        path.push_back(at);
    path.push_back(startPoly);
    std::reverse(path.begin(), path.end());
    return path;
}

FVector NavMesh::GetRandomPosition() const
{
    if (polygons.empty() || vertices.empty())
        return FVector::ZeroVector;

    int polyCount = static_cast<int>(polygons.size());
    int polyIdx = RandomUtils::GetRandomInt(0, polyCount - 1);
    const auto& poly = polygons[polyIdx];
    int vcount = static_cast<int>(poly.size());
    if (vcount < 3)
        return FVector::ZeroVector;

    int triCount = vcount - 2;
    int ti = RandomUtils::GetRandomInt(0, triCount - 1);

    const FVector& A = vertices[poly[0]];
    const FVector& B = vertices[poly[ti + 1]];
    const FVector& C = vertices[poly[ti + 2]];

    float u = RandomUtils::GetRandomFloat(0.f, 1.f);
    float v = RandomUtils::GetRandomFloat(0.f, 1.f);
    if (u + v > 1.f) {
        u = 1.f - u;
        v = 1.f - v;
    }

    FVector P = A + (B - A) * u + (C - A) * v;
    P.Z += 90;
    return P;
}