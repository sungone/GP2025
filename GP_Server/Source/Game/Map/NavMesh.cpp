#include "pch.h"
#include "NavMesh.h"
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

int NavMesh::FindClosestPoly(const FVector& pos) const
{
    int bestIdx = -1;
    float bestDist = std::numeric_limits<float>::infinity();

    for (int i = 0; i < static_cast<int>(polygons.size()); ++i)
    {
        FVector center(0, 0, 0);
        for (int vid : polygons[i])
            center = center + vertices[vid];
        center = center / static_cast<float>(polygons[i].size());

        float distSq = center.DistanceSquared2D(pos);
        if (distSq < bestDist)
        {
            bestDist = distSq;
            bestIdx = i;
        }
    }

    return bestIdx;
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

std::vector<int> NavMesh::FindPathAStar(const FVector& startPos, const FVector& goalPos) const
{
    int startPoly = FindIdxFromPos(startPos);
    int goalPoly = FindIdxFromPos(goalPos);

    if (startPoly < 0)
        startPoly = FindClosestPoly(startPos);
    if (goalPoly < 0)
        goalPoly = FindClosestPoly(goalPos);

    if (startPoly < 0 || goalPoly < 0)
        return {};


    int N = static_cast<int>(polygons.size());
    // Precompute polygon centers
    std::vector<FVector> centers(N);
    for (int i = 0; i < N; ++i) {
        FVector sum(0, 0, 0);
        for (int vid : polygons[i])
            sum = sum + vertices[vid];
        centers[i] = sum / static_cast<float>(polygons[i].size());
    }

    auto Heuristic = [&](int p) {
        return (centers[p] - goalPos).Length();
        };

    struct Node { int poly; float g, f; int parent; };
    struct Compare { bool operator()(const Node& a, const Node& b) const { return a.f > b.f; } };

    std::priority_queue<Node, std::vector<Node>, Compare> openPQ;
    std::vector<float>       gScore(N, std::numeric_limits<float>::infinity());
    std::vector<int>         parent(N, -1);
    std::vector<bool>        closed(N, false);

    // Initialize start
    gScore[startPoly] = (startPos - centers[startPoly]).Length();
    openPQ.push({ startPoly, gScore[startPoly], gScore[startPoly] + Heuristic(startPoly), -1 });

    while (!openPQ.empty()) {
        Node cur = openPQ.top();
        openPQ.pop();
        if (closed[cur.poly]) continue;
        closed[cur.poly] = true;
        parent[cur.poly] = cur.parent;
        if (cur.poly == goalPoly) break;

        for (int nb : neighbors[cur.poly]) {
            if (closed[nb]) continue;
            float cost = (centers[cur.poly] - centers[nb]).Length();
            float tentativeG = gScore[cur.poly] + cost;
            if (tentativeG < gScore[nb]) {
                gScore[nb] = tentativeG;
                openPQ.push({ nb, tentativeG, tentativeG + Heuristic(nb), cur.poly });
            }
        }
    }

    if (parent[goalPoly] < 0)
        return {};

    std::vector<int> path;
    for (int at = goalPoly; at != -1; at = parent[at])
        path.push_back(at);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<FVector> NavMesh::GetStraightPath(
    const FVector& startPos,
    const FVector& goalPos,
    const std::vector<int>& polyPath) const
{
    std::vector<FVector> path;
    if (polyPath.size() < 2)
    {
        path.push_back(startPos);
        path.push_back(goalPos);
        return path;
    }

    std::vector<std::pair<FVector, FVector>> portals;
    portals.emplace_back(startPos, startPos);
    for (size_t i = 0; i + 1 < polyPath.size(); ++i)
    {
        const auto& A = polygons[polyPath[i]];
        const auto& B = polygons[polyPath[i + 1]];
        std::unordered_set<int> setA(A.begin(), A.end());
        std::vector<FVector> shared;
        shared.reserve(2);
        for (int vid : B)
            if (setA.count(vid))
                shared.push_back(vertices[vid]);

        if (shared.size() == 2)
        {
            FVector L = shared[0], R = shared[1];
            auto cross2D = [](const FVector& a, const FVector& b) {
                return a.X * b.Y - a.Y * b.X;
                };
            if (cross2D(R - L, goalPos - L) < 0)
                std::swap(L, R);

            portals.emplace_back(L, R);
        }
    }
    portals.emplace_back(goalPos, goalPos);

    path.reserve(portals.size());
    path.push_back(startPos);
    for (size_t i = 1; i + 1 < portals.size(); ++i)
    {
        const auto& [L, R] = portals[i];
        FVector mid = (L + R) * 0.5;
        path.push_back(mid);
    }
    path.push_back(goalPos);

    return path;
}
